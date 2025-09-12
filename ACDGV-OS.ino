#include <FS.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_seesaw.h>
#include <DFRobot_GR10_30.h>
#include <DFRobot_MAX17043.h>
#include <DFRobot_VisualRotaryEncoder.h>
#include <DFRobot_BloodOxygen_S.h>
#include <SparkFun_Qwiic_Keypad_Arduino_Library.h>

const String version = "8";
const int devicesCount = 6;
const int programsCount = 6;

int channelsCount;
int surfacesCount;

#include "./engine/I2C.h";
#include "./engine/Program.h";
#include "./engine/Device.h";
#include "./engine/Storage.h";

I2C i2c = I2C();
Program * programs[programsCount];
Device * devices[devicesCount];

#include "./devices/AM.h";
#include "./devices/GV.h";
#include "./devices/HRS.h";
#include "./devices/RE.h";
#include "./devices/Gamepad.h";
#include "./devices/Keypad.h";

AM accelerometer = AM(0x53);
GV gv = GV();
HRS hrs = HRS(0x57);
RE re = RE(0x55);
Gamepad gamepad = Gamepad(0x51);
Keypad keypad = Keypad();

#include "./engine/Channel.h";
Channel * channels;

#include "./engine/Surface.h";
Surface * surfaces;

#include "./engine/Interface.h";
Interface interface;

#include "./programs/Batterfly.h";
#include "./programs/Gravity.h";
#include "./programs/VV.h";
#include "./programs/Logo.h";
#include "./programs/Telephone.h";
#include "./programs/I2c.h";

void setup() {
  Serial.begin(9600);
  SPIFFS.begin();
  i2c.init();

  JsonArray & configSurfaces = loadFromFile("/config/surfaces/" + version + ".json");
  surfacesCount = configSurfaces.size();
  channelsCount = Surface::countChannels(configSurfaces);

  channels = new Channel[channelsCount];
  for (int i = 0; i < channelsCount; i++) {
    channels[i].init(i);
  }

  devices[0] = &accelerometer;
  devices[1] = &gv;
  devices[2] = &hrs;
  devices[3] = &re;
  devices[4] = &gamepad;
  devices[5] = &keypad;
  for (int i = 0; i < devicesCount; i++) {
    devices[i]->init();
  }

  surfaces = new Surface[surfacesCount];
  for (int i=0; i<surfacesCount; i++) {
    surfaces[i] = Surface::createFromConfigFile(configSurfaces[i]);
  }
  
  for (int i = 0; i < surfacesCount; i++) {
    surfaces[i].init();
    surfaces[i].countScreens();
  }

  programs[0] = new Batterfly();
  programs[1] = new Gravity();
  programs[2] = new VV();
  programs[3] = new Logo();
  programs[4] = new Telephone();
  programs[5] = new I2c();

  interface.init();
}

void loop() {
  for (int i = 0; i < devicesCount; i++) {
    devices[i]->tick();
  }

  for (int c = 0; c < channelsCount; c++) {
    channels[c].tick();
  }

  for (int i = 0; i < surfacesCount; i++) {
    surfaces[i].tick();
  }

  interface.tick();

  for (int c = 0; c < channelsCount; c++) {
    channels[c].display();
  }
}
