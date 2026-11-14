#pragma once

#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#define DHT_PIN D5
#define DHT_TYPE DHT11
#define BUTTON_PIN D6
#define LED_PIN D7
#define POTENTIOMETER_PIN A0

#define SDA_PIN D2
#define SCL_PIN D1

void initializeSerial();
void initializeSensors();
void checkSerialCommands();
void checkSwitch();
void checkTilt();
void readPotentiometer();
void readDHT();
bool connectToWiFi();
void initializeMqtt();
void serviceMqtt();
