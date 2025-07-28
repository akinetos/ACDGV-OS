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
int activeProgram = -1;

#include "./engine/I2C.h";
I2C i2c = I2C();

#include "./engine/Program.h";
Program* programs[programsCount];

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
Batterfly batterfly = Batterfly();

#include "./programs/Router.h";
Router router = Router();

#include "./programs/Gravity.h";
Gravity gravity = Gravity();

#include "./programs/VV.h";
VV vv = VV();

#include "./programs/Logo.h";
Logo logo = Logo();

#include "./programs/Telephone.h";
Telephone telephone = Telephone();


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
  wifi.init();

  surfaces[0] = Surface(128, 64, 128, 32, 1, 1, 0);
  surfaces[1] = Surface(128, 256, 128, 32, -1, 1, 1);
  surfaces[2] = Surface(128, 64, 128, 64, 1, 1, 0);
  for (int i = 0; i < 3; i++) {
    surfaces[i].init();
  }

  programs[0] = &batterfly;
  programs[1] = &gravity;
  programs[2] = &vv;
  programs[3] = &router;
  programs[4] = &logo;
  programs[5] = &telephone;

  interface.init();
}


void loop() {
  for (int i = 0; i < sensorsCount; i++) {
    sensors[i]->tick();
  }

  for (int i = 0; i < channelsCount; i++) {
    channels[i].tick("sensors");
  }

  for (int i = 0; i < 3; i++) {
    surfaces[i].tick();
  }

  if (activeProgram > -1 && activeProgram < programsCount) {
    programs[activeProgram]->tick();
  }

  interface.tick();

  for (int i = 0; i < channelsCount; i++) {
    channels[i].tick("screens");
  }
}
