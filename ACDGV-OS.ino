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
#include <PN532_I2C.h>
#include <NfcAdapter.h>

const String version = "8";
const int devicesCount = 8;
const int programsCount = 10;

int transitionType = 0;
String action = "";
int channelsCount;
int surfacesCount;
int activeProgram = 9;

DynamicJsonBuffer jsonBuffer;

#include "./engine/Storage.h";
Storage storage = Storage();

#include "./engine/I2C.h";
I2C i2c = I2C();

#include "./engine/Program.h";
Program * programs[programsCount];

#include "./engine/Device.h";
Device * devices[devicesCount];

#include "./engine/Pixel.h";
#include "./engine/OLED.h";
#include "./engine/Port.h";
#include "./engine/Channel.h";
Channel * channels;

#include "./engine/Surface.h";
Surface * surfaces;

#include "./engine/Transition.h";
Transition transition = Transition();

#include "./engine/Menu.h";
Menu menu;

#include "./devices/AM.h";
#include "./devices/GV.h";
#include "./devices/HRS.h";
#include "./devices/RE.h";
#include "./devices/Gamepad.h";
#include "./devices/Keypad.h";
#include "./devices/GD.h";
#include "./devices/NFC.h";
#include "./devices/DistanceSensor.h";

HRS hrs = HRS(0x57);
RE re = RE(0x55);
Keypad keypad = Keypad();
GD gd = GD();
GV gv = GV();

#include "./programs/Batterfly.h";
#include "./programs/Gravity.h";
#include "./programs/VV.h";
#include "./programs/Logo.h";
#include "./programs/Telephone.h";
#include "./programs/I2c.h";
#include "./programs/Contacts.h";
#include "./programs/NFC.h";
#include "./programs/Battery.h";
#include "./programs/Skaner3d.h";

void setup() {
  Serial.begin(9600);
  storage.init();
  i2c.init();

  const String path = "/config/surfaces/" + version + ".json";
  JsonArray & config = storage.load(path);
  surfacesCount = config.size();
  channelsCount = Channel::count(config);

  channels = new Channel[channelsCount];
  for (int i = 0; i < channelsCount; i++)
    channels[i].init(i);

  devices[0] = new AM(0x1D);
  devices[1] = new Gamepad(0x51);
  devices[2] = new NFCDevice();
  devices[3] = &gv;
  devices[4] = &hrs;
  devices[5] = &re;
  devices[6] = &keypad;
  devices[7] = &gd;
  for (int i = 0; i < devicesCount; i++)
    devices[i]->init();

  for (int i = 0; i<8; i++) {
    channels[0].ports[i].devices[0] = new DistanceSensor(0x29);
  }

  surfaces = new Surface[surfacesCount];
  for (int i = 0; i < surfacesCount; i++) {
    Surface * surface = new Surface();
    surface->init(config[i]);
    surfaces[i] = * surface;
  }

  programs[0] = new Batterfly();
  programs[1] = new Gravity();
  programs[2] = new VV();
  programs[3] = new Logo();
  programs[4] = new Telephone();
  programs[5] = new I2c();
  programs[6] = new Contacts();
  programs[7] = new NFCProgram();
  programs[8] = new Battery();
  programs[9] = new Skaner3d();

  transition = Transition();

  menu.init();

  if (activeProgram > -1) {
    programs[activeProgram]->init();
    programs[activeProgram]->activate();
  }
}

void loop() {
  for (int i = 0; i < devicesCount; i++)
    devices[i]->tick();

  for (int i = 0; i < 8; i++) {
    channels[0].ports[i].tick();
  }

  for (int i = 0; i < surfacesCount; i++)
    surfaces[i].tick();

  menu.tick();
  transition.tick();

  for (int i = 0; i < programsCount; i++)
    programs[i]->tick();

  for (int i = 0; i < surfacesCount; i++)
    surfaces[i].clear();

  for (int i = 0; i < programsCount; i++)
    programs[i]->draw();

  menu.draw();
  transition.draw();

  for (int i = 0; i < surfacesCount; i++)
    surfaces[i].draw();
}
