#include <FS.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_seesaw.h>
#include <DFRobot_GR10_30.h>
#include <DFRobot_MAX17043.h>
#include <DFRobot_VisualRotaryEncoder.h>
#include <DFRobot_BloodOxygen_S.h>
#include <SparkFun_Qwiic_Keypad_Arduino_Library.h>


const int channelsCount = 2;
const int sensorsCount = 6;
const int programsCount = 6;
const int surfacesCount = 3;

#include "./engine/I2C.h";
I2C i2c = I2C();

#include "./engine/Program.h";
Program * programs[programsCount];

#include "./engine/Device.h";
Device* sensors[sensorsCount];

#include "./devices/WiFi.h";
Wifi wifi = Wifi();

#include "./devices/AM.h";
AM accelerometer = AM(0x1d);

#include "./devices/GV.h";
GV gv = GV();

#include "./devices/HRS.h";
HRS hrs = HRS(0x57);

#include "./devices/RE.h";
RE re = RE(0x55);

#include "./devices/Gamepad.h";
Gamepad gamepad = Gamepad(0x51);

#include "./devices/Keypad.h";
Keypad keypad = Keypad();

#include "./engine/Channel.h";
Channel channels[channelsCount];

#include "./engine/Surface.h";
Surface surfaces[3];

#include "./engine/Interface.h";
Interface interface;

#include "./programs/Batterfly.h";
#include "./programs/Router.h";
#include "./programs/Gravity.h";
#include "./programs/VV.h";
#include "./programs/Logo.h";
#include "./programs/Telephone.h";


void setup() {
  SPIFFS.begin();
  i2c.init();

  for (int i = 0; i < channelsCount; i++) {
    channels[i].init(i);
  }
  channels[0].ports[5].init("sensors");
  channels[0].ports[7].init("sensors");

  sensors[0] = &accelerometer;
  sensors[1] = &gv;
  sensors[2] = &hrs;
  sensors[3] = &re;
  sensors[4] = &gamepad;
  sensors[5] = &keypad;
  for (int i = 0; i < sensorsCount; i++) {
    sensors[i]->init();
  }

  surfaces[0] = Surface(128, 64, 128, 32, 1, 1, 0);
  surfaces[1] = Surface(128, 256, 128, 32, -1, 1, 1);
  surfaces[2] = Surface(128, 64, 128, 64, 1, 1, 0);
  for (int i = 0; i < surfacesCount; i++) {
    surfaces[i].init();
  }

  programs[0] = new Batterfly();
  programs[1] = new Gravity();
  programs[2] = new VV();
  programs[3] = new Router();
  programs[4] = new Logo();
  programs[5] = new Telephone();

  interface.init();
}


void loop() {
  for (int i = 0; i < sensorsCount; i++) {
    sensors[i]->tick();
  }

  for (int i = 0; i < channelsCount; i++) {
    channels[i].tick("sensors");
  }

  for (int i = 0; i < surfacesCount; i++) {
    surfaces[i].tick();
  }

  interface.tick();

  for (int i = 0; i < channelsCount; i++) {
    channels[i].tick("screens");
  }
}
