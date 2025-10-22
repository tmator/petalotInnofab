#include "pins.hpp"
#include "conf.hpp"
#include "wifi.hpp"
#include "stepper.hpp"
#include "hotend.hpp"
#include "server.hpp"
#include "ota.hpp"

//SSD1306Wire display(0x3c, D6, D7); 

void setup() {
  Serial.begin(115200);
  delay(1000);
  initConf();
  initWiFi();
  initOTA();
  initHotend();
  initStepper();
  InitServer();
  //InitDisplay();
}

void loop() {
  wifiTask();
  server.handleClient();
  hotendReadTempTask();
  stepperRunTask();
  ArduinoOTA.handle();
  readConfigurationSerial();
  //DisplayInfo();
}
