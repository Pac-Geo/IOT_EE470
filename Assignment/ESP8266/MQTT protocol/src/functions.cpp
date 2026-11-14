#include <ESP8266WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>

#include "functions.h"
#include "Contra.h"

namespace {

const char* MQTT_SERVER = "broker.hivemq.com";
const uint16_t MQTT_PORT = 1883;
const char* MQTT_PUBLISH_TOPIC =
    "testtopic/temp/outTopic/EE470pacgeo8266";
const char* MQTT_POTENTIOMETER_TOPIC =
    "testtopic/temp/outTopic/EE470pacgeo8266/potentiometer";
const char* MQTT_SUBSCRIBE_TOPIC = "testtopic/temp/inTopic/pacgeo";
const unsigned long MQTT_RECONNECT_INTERVAL_MS = 30000UL;
const unsigned long BUTTON_RELEASE_DELAY_MS = 5000UL;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

float currentTemperature = 0.0;
float currentHumidity = 0.0;
bool dhtReadingValid = false;

DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_ADXL345_Unified accelerometer =
    Adafruit_ADXL345_Unified(12345);

bool adxlReady = false;
bool lastButtonState = HIGH;
bool tiltActive = false;

int lastPotentiometerValue = -1;
int currentPotentiometerValue = 0;
int pendingPotentiometerValue = 0;
bool potentiometerPublishPending = false;
bool buttonPressPublishPending = false;
bool buttonReleasePublishPending = false;
unsigned long buttonReleasePublishAt = 0;
unsigned long lastDhtRead = 0;
unsigned long lastAccelRead = 0;
unsigned long lastPotentiometerRead = 0;
unsigned long lastMqttReconnectAttempt = 0;
unsigned long lastWiFiReconnectAttempt = 0;

}  // namespace

void mqttMessageReceived(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, MQTT_SUBSCRIBE_TOPIC) != 0 || length != 1) {
    return;
  }

  if (payload[0] == '1') {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("MQTT command received: LED ON");
  } else if (payload[0] == '0') {
    digitalWrite(LED_PIN, LOW);
    Serial.println("MQTT command received: LED OFF");
  }
}

void initializeSerial() {
  // Start UART serial communication with the computer at 115200 baud.
  // Set the Serial Monitor to the same baud rate so the text is readable.
  Serial.begin(115200);
  delay(3000);

  Serial.println();
  Serial.println();
  Serial.println("EE 470");
}

void initializeSensors() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  dht.begin();

  Wire.begin(SDA_PIN, SCL_PIN);

  if (accelerometer.begin()) {
    adxlReady = true;
    accelerometer.setRange(ADXL345_RANGE_16_G);
    Serial.println("ADXL345 detected.");
  } else {
    adxlReady = false;
    Serial.println("ADXL345 not detected.");
  }

  Serial.println("DHT11 started.");
  Serial.println("Enter 1 to turn the D7 LED on, or 0 to turn it off.");
  Serial.println("Press the button or tilt the ADXL345.");
}

bool connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection started; waiting for connection.");
    return false;
  }

  Serial.println("WiFi connected.");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");

  return true;
}

void initializeMqtt() {
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttMessageReceived);
  Serial.print("MQTT publish topic: ");
  Serial.println(MQTT_PUBLISH_TOPIC);
  Serial.print("MQTT subscribe topic: ");
  Serial.println(MQTT_SUBSCRIBE_TOPIC);
}

void serviceMqtt() {
  unsigned long now = millis();

  if (WiFi.status() != WL_CONNECTED) {
    if (now - lastWiFiReconnectAttempt >= MQTT_RECONNECT_INTERVAL_MS) {
      lastWiFiReconnectAttempt = now;
      Serial.println("WiFi disconnected. Reconnecting...");
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
    return;
  }

  if (!mqttClient.connected()) {
    if (now - lastMqttReconnectAttempt >= MQTT_RECONNECT_INTERVAL_MS) {
      lastMqttReconnectAttempt = now;

      String clientId = "ESP8266-";
      clientId += String(ESP.getChipId(), HEX);

      Serial.print("Connecting to MQTT broker...");
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("connected.");
        if (!mqttClient.subscribe(MQTT_SUBSCRIBE_TOPIC)) {
          Serial.println("MQTT subscription failed.");
        }
      } else {
        Serial.print("failed, state=");
        Serial.println(mqttClient.state());
      }
    }
    return;
  }

  mqttClient.loop();

  if (buttonPressPublishPending) {
    if (mqttClient.publish(MQTT_PUBLISH_TOPIC, "1")) {
      Serial.println("Published button state: 1");
      buttonReleasePublishAt = now + BUTTON_RELEASE_DELAY_MS;
      buttonReleasePublishPending = true;
    } else {
      Serial.println("Could not publish button state 1.");
    }

    buttonPressPublishPending = false;
  }

  if (potentiometerPublishPending) {
    char payload[12];
    snprintf(payload, sizeof(payload), "%d", pendingPotentiometerValue);

    if (mqttClient.publish(MQTT_POTENTIOMETER_TOPIC, payload)) {
      Serial.print("Published potentiometer ADC: ");
      Serial.println(payload);
    } else {
      Serial.println("Could not publish potentiometer reading.");
    }

    potentiometerPublishPending = false;
  }

  if (buttonReleasePublishPending &&
      static_cast<long>(now - buttonReleasePublishAt) >= 0) {
    if (mqttClient.publish(MQTT_PUBLISH_TOPIC, "0")) {
      Serial.println("Published button state: 0");
    } else {
      Serial.println("Could not publish button state 0.");
    }

    buttonReleasePublishPending = false;
  }

}

void checkSerialCommands() {
  if (Serial.available() > 0) {
    // Read one character sent from the Serial Monitor over the UART connection.
    char input = Serial.read();

    if (input == '1') {
      digitalWrite(LED_PIN, HIGH);
      // Send confirmation back to the computer over the same serial connection.
      Serial.println("LED: 1 (ON)");
    } else if (input == '0') {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED: 0 (OFF)");
    }
  }
}

void checkSwitch() {
  bool currentButtonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    // Report the button press through UART so it appears in the Serial Monitor.
    Serial.println("1");
    Serial.println("BUTTON ACTIVATED - Node_1 event");

    // Queue one press message and one potentiometer sample for this button press.
    buttonPressPublishPending = true;
    pendingPotentiometerValue = analogRead(POTENTIOMETER_PIN);
    potentiometerPublishPending = true;

    delay(250);
  }

  lastButtonState = currentButtonState;
}

void checkTilt() {
  if (!adxlReady || millis() - lastAccelRead < 500UL) {
    return;
  }

  lastAccelRead = millis();

  sensors_event_t event;
  accelerometer.getEvent(&event);

  float horizontalAcceleration =
      sqrtf(event.acceleration.x * event.acceleration.x +
            event.acceleration.y * event.acceleration.y);

  Serial.print("Acceleration X: ");
  Serial.print(event.acceleration.x);
  Serial.print(" | Y: ");
  Serial.print(event.acceleration.y);
  Serial.print(" | Z: ");
  Serial.print(event.acceleration.z);
  Serial.println(" m/s^2");

  bool currentlyTilted;

  if (tiltActive) {
    currentlyTilted = horizontalAcceleration > 1.0f;
  } else {
    currentlyTilted = horizontalAcceleration > 1.5f;
  }

  if (currentlyTilted && !tiltActive) {
    tiltActive = true;

    Serial.println("TILT ACTIVATED - Node_2 event");
  }

  if (!currentlyTilted && tiltActive) {
    tiltActive = false;
    Serial.println("Tilt cleared. Ready for next activation.");
  }
}

void readPotentiometer() {
  if (millis() - lastPotentiometerRead < 100UL) {
    return;
  }

  lastPotentiometerRead = millis();
  // analogRead converts A0 voltage to a 10-bit count: near 0 V gives 0,
  // about 1.5v gives 512, and near full scale gives 1023.
  // The supported voltage range depends on the NodeMCU board's A0 circuit.
  int value = analogRead(POTENTIOMETER_PIN);
  currentPotentiometerValue = value;

  if (value != lastPotentiometerValue) {
    lastPotentiometerValue = value;
    // Print the ADC count as a decimal number, not as the voltage itself.
    Serial.print("Potentiometer ADC (0-1023): ");
    Serial.println(value);
  }
}

void readDHT() {
  if (millis() - lastDhtRead < 2500UL) {
    return;
  }

  lastDhtRead = millis();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("DHT11 read failed.");
    return;
  }

  currentTemperature = temperature;
  currentHumidity = humidity;
  dhtReadingValid = true;

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C | Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
}
