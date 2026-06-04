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
const int programsCount = 9;
const int transitionsCount = 1;

String action = "";
int channelsCount;
int surfacesCount;
boolean transition = false;
int activeProgram = -1;

DynamicJsonBuffer jsonBuffer;

#include "./engine/Storage.h";
Storage storage = Storage();

#include "./engine/I2C.h";
I2C i2c = I2C();

#include "./engine/Program.h";
Program * programs[programsCount];

boolean anyProgramActive() {
  return activeProgram != -1;
}

#include "./engine/Device.h";
Device * devices[devicesCount];

#include "./engine/OLED.h";
#include "./engine/Port.h";
#include "./engine/Channel.h";
Channel * channels;

#include "./engine/Surface.h";
Surface * surfaces;

#include "./transitions/fall.h";
#include "./transitions/slide.h";
FallTransition * transitions[transitionsCount];

#include "./engine/Menu.h";
Menu menu;

void execute(JsonArray & command) {
  String commandType = command[0];

  if (commandType == "run") {
    String programName = command[1];
    int programIndex = -1;
    for (int i=0; i<programsCount; i++)
      if (programs[i]->name == programName)
        programIndex = i;
    
    if (programIndex > -1) {
      boolean hasOption = command.size() == 3;
      boolean isActive = programs[programIndex]->active;

      if (hasOption) {
        int optionValue = command[2];
        programs[programIndex]->setOption(optionValue);
      }

      if (!isActive) {
        activeProgram = programIndex;
        if (!programs[activeProgram]->initialised)
          programs[activeProgram]->init();
        programs[activeProgram]->activate();
        programs[activeProgram]->menuLevel = menu.level;
      }
    }
  }
}

#include "./devices/AM.h";
AM accelerometer = AM(0x1D);

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

#include "./devices/GD.h";
GD gd = GD();

#include "./devices/NFC.h";
NFCDevice nfcDevice = NFCDevice();

#include "./programs/Batterfly.h";
#include "./programs/Gravity.h";
#include "./programs/VV.h";
#include "./programs/Logo.h";
#include "./programs/Telephone.h";
#include "./programs/I2c.h";
#include "./programs/Contacts.h";
#include "./programs/NFC.h";
#include "./programs/Battery.h";

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

  devices[0] = &accelerometer;
  devices[1] = &gv;
  devices[2] = &hrs;
  devices[3] = &re;
  devices[4] = &gamepad;
  devices[5] = &keypad;
  devices[6] = &gd;
  devices[7] = &nfcDevice;
  for (int i = 0; i < devicesCount; i++)
    devices[i]->init();

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

  transitions[0] = new FallTransition();

  menu.init();
}

void loop() {
  for (int i = 0; i < devicesCount; i++)
    devices[i]->tick();

  for (int i = 0; i < surfacesCount; i++)
    surfaces[i].tick(devices[4]);

  menu.tick();
  
  for (int i = 0; i < transitionsCount; i++)
    transitions[i]->tick();

  for (int i = 0; i < programsCount; i++)
    programs[i]->tick();

  for (int i = 0; i < surfacesCount; i++)
    surfaces[i].clear();

  for (int i = 0; i < transitionsCount; i++)
    transitions[i]->draw();

  for (int i = 0; i < programsCount; i++)
    programs[i]->draw();
  
  for (int i = 0; i < surfacesCount; i++)
    surfaces[i].draw(menu.level);
}
