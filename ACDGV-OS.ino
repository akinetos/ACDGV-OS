#include <FS.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

const int programsCount = 1;

#include "./engine/I2C.h";
I2C i2c = I2C();

#include "./engine/Program.h";
Program * programs[programsCount];

#include "./engine/Device.h";
#include "./devices/WiFi.h";
Wifi wifi = Wifi();

#include "./programs/Router.h";

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();
  i2c.init();
  programs[0] = new Router();
  programs[0]->init();
  programs[0]->active = true;
}

void loop() {
  for (int i=0; i<programsCount; i++) {
    if (programs[i]->active) {
      programs[i]->tick();
    }
  }
}
