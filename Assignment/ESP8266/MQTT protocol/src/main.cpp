#include "functions.h"

void setup() {
  initializeSerial();
  initializeSensors();
  connectToWiFi();
  initializeMqtt();
}

void loop() {
  checkSerialCommands();
  checkSwitch();
  readPotentiometer();
  serviceMqtt();
}