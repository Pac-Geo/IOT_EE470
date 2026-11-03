#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#define DHT_PIN D5
#define DHT_TYPE DHT11
#define BUTTON_PIN D6

#define SDA_PIN D2
#define SCL_PIN D1

#include "Contra.h"

#define SENSOR_ENDPOINT \
  "https://lawngreen-grasshopper-862494.hostingersite.com/dashboard.php"

#define RETRY_INTERVAL_MS 30000UL

float currentTemperature = 0.0;
float currentHumidity = 0.0;
bool dhtReadingValid = false;

const char* timeZoneNames[] = {
  "America/New_York",
  "America/Chicago",
  "America/Denver",
  "America/Los_Angeles",
  "America/Anchorage",
  "Pacific/Honolulu",
  "America/Puerto_Rico"
};

const char* timeZoneLabels[] = {
  "Eastern Time - New York",
  "Central Time - Chicago",
  "Mountain Time - Denver",
  "Pacific Time - Los Angeles",
  "Alaska Time - Anchorage",
  "Hawaii-Aleutian Time - Honolulu",
  "Atlantic Time - Puerto Rico"
};

int selectedTimeZone = 4;

DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_ADXL345_Unified accelerometer =
    Adafruit_ADXL345_Unified(12345);

bool adxlReady = false;
bool lastButtonState = HIGH;
bool tiltActive = false;

unsigned long lastDhtRead = 0;
unsigned long lastAccelRead = 0;

struct PendingMessage {
  bool active = false;
  String nodeName;
  String timestamp;
  float temperature = 0.0;
  float humidity = 0.0;
  unsigned long lastAttempt = 0;
};

PendingMessage pendingNode1;
PendingMessage pendingNode2;

void initializeSerial();
void initializeSensors();
void checkPushbutton();
void readDHT();
void readAccelerometer();
void selectTimeZone();
bool connectToWiFi();

String getCurrentTimeFromAPI();

bool sendSensorData(const char* nodeName);
bool transmitMessage(const PendingMessage& message);
void retryPendingMessages();

void setup() {
  initializeSerial();
  selectTimeZone();
  initializeSensors();
  connectToWiFi();
}

void loop() {
  checkPushbutton();
  readDHT();
  readAccelerometer();
  retryPendingMessages();
}

void initializeSerial() {
  Serial.begin(115200);
  delay(3000);

  Serial.println();
  Serial.println();
  Serial.println("EE 470");
}

void initializeSensors() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

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
  Serial.println("Press the button or tilt the ADXL345.");
}

void checkPushbutton() {
  bool currentButtonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    Serial.println("BUTTON ACTIVATED - Node_1 event");

    if (sendSensorData("node_1")) {
      Serial.println("Node_1 data accepted by server.");
    } else {
      Serial.println("Node_1 data saved for retry.");
    }

    delay(250);
  }

  lastButtonState = currentButtonState;
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

void readAccelerometer() {
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

  // New tilt activation: send exactly once.
  if (currentlyTilted && !tiltActive) {
    tiltActive = true;

    Serial.println("TILT ACTIVATED - Node_2 event");

    if (sendSensorData("node_2")) {
      Serial.println("Node_2 data accepted by server.");
    } else {
      Serial.println("Node_2 data saved for retry.");
    }
  }

  // Return to upright position before another activation is allowed.
  if (!currentlyTilted && tiltActive) {
    tiltActive = false;
    Serial.println("Tilt cleared. Ready for next activation.");
  }
}

void selectTimeZone() {
  selectedTimeZone = 4;

  unsigned long startTime = millis();
  unsigned long lastMenuPrint = 0;

  while (millis() - startTime < 60000UL) {
    if (millis() - lastMenuPrint >= 1000UL) {
      Serial.println();
      Serial.println("Select Your Time Zone:");
      Serial.println("1. Eastern Time - New York");
      Serial.println("2. Central Time - Chicago");
      Serial.println("3. Mountain Time - Denver");
      Serial.println("4. Pacific Time - Los Angeles");
      Serial.println("5. Alaska Time - Anchorage");
      Serial.println("6. Hawaii-Aleutian Time - Honolulu");
      Serial.println("7. Atlantic Time - Puerto Rico");
      Serial.println("Enter 1-7. Default is 4 after 60 seconds.");

      lastMenuPrint = millis();
    }

    if (Serial.available()) {
      char input = Serial.read();

      if (input >= '1' && input <= '7') {
        selectedTimeZone = input - '0';
        break;
      }
    }

    delay(10);
  }

  Serial.print("Selected timezone: ");
  Serial.println(timeZoneLabels[selectedTimeZone - 1]);
}

bool connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startTime < 20000UL) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi connection failed.");
    return false;
  }

  Serial.println();
  Serial.println("WiFi connected.");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");

  return true;
}

String getCurrentTimeFromAPI() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi is not connected.");
    return "";
  }

  String url =
      "https://timeapi.io/api/Time/current/zone?timeZone=" +
      String(timeZoneNames[selectedTimeZone - 1]);

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  if (!http.begin(client, url)) {
    Serial.println("Could not connect to TimeAPI.");
    return "";
  }

  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK) {
    Serial.print("TimeAPI HTTP error: ");
    Serial.println(httpCode);
    http.end();
    return "";
  }

  String response = http.getString();
  http.end();

  JsonDocument document;
  DeserializationError error = deserializeJson(document, response);

  if (error) {
    Serial.println("Could not decode TimeAPI response.");
    return "";
  }

  String dateTime = document["dateTime"] | "";

  if (dateTime.length() < 19) {
    Serial.println("Invalid timestamp received.");
    return "";
  }

  dateTime = dateTime.substring(0, 19);
  dateTime.setCharAt(10, ' ');

  return dateTime;
}

bool sendSensorData(const char* nodeName) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi is not connected.");
    return false;
  }

  if (!dhtReadingValid) {
    Serial.println("No valid DHT11 reading available.");
    return false;
  }

  // Get the timestamp once and preserve it for retries.
  String timestamp = getCurrentTimeFromAPI();

  if (timestamp.length() == 0) {
    Serial.println("Timestamp could not be obtained.");
    return false;
  }

  PendingMessage message;
  message.active = true;
  message.nodeName = nodeName;
  message.timestamp = timestamp;
  message.temperature = currentTemperature;
  message.humidity = currentHumidity;
  message.lastAttempt = millis();

  // Attempt immediate transmission.
  if (transmitMessage(message)) {
    return true;
  }

  // Save the original message for retry.
  if (String(nodeName) == "node_1") {
    pendingNode1 = message;
  } else {
    pendingNode2 = message;
  }

  Serial.println("Message saved. Retry will occur in 30 seconds.");
  return false;
}

bool transmitMessage(const PendingMessage& message) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi is not connected. Retry postponed.");
    return false;
  }

  String encodedTimestamp = message.timestamp;
  encodedTimestamp.replace(" ", "%20");
  encodedTimestamp.replace(":", "%3A");

  String url = String(SENSOR_ENDPOINT) +
               "?node_name=" + message.nodeName +
               "&time_received=" + encodedTimestamp +
               "&temperature=" + String(message.temperature, 2) +
               "&humidity=" + String(message.humidity, 2);

  Serial.println("Sending measurement:");
  Serial.println(url);

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  if (!http.begin(client, url)) {
    Serial.println("Could not start HTTP request.");
    return false;
  }

  int httpCode = http.GET();
  String response = http.getString();

  Serial.print("HTTP status: ");
  Serial.println(httpCode);
  Serial.println(response);

  http.end();

  // 200 = stored successfully.
  // 409 = duplicate already stored, so delivery is complete.
  return httpCode == 200 ||
         httpCode == 202 ||
         httpCode == 204 ||
         httpCode == 409;
}

void retryPendingMessages() {
  unsigned long now = millis();

  if (pendingNode1.active &&
      now - pendingNode1.lastAttempt >= RETRY_INTERVAL_MS) {

    Serial.println("Retrying Node_1 message...");

    pendingNode1.lastAttempt = now;

    if (transmitMessage(pendingNode1)) {
      Serial.println("Node_1 retry accepted.");
      pendingNode1.active = false;
    } else {
      Serial.println("Node_1 retry failed. Retrying again in 30 seconds.");
    }
  }

  if (pendingNode2.active &&
      now - pendingNode2.lastAttempt >= RETRY_INTERVAL_MS) {

    Serial.println("Retrying Node_2 message...");

    pendingNode2.lastAttempt = now;

    if (transmitMessage(pendingNode2)) {
      Serial.println("Node_2 retry accepted.");
      pendingNode2.active = false;
    } else {
      Serial.println("Node_2 retry failed. Retrying again in 30 seconds.");
    }
  }
}
