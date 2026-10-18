#include "ArduinoJson.h"

#include "./silnik/I2C.h";
I2C i2c = I2C();

#include "./silnik/Storage.h";
Storage storage = Storage();

#include "./silnik/Device.h";
#include "./czujniki/AM.h";
#include "./czujniki/Gamepad.h";
#include "./czujniki/Keypad.h";
#include "./czujniki/NFC.h";
#include "./czujniki/DistanceSensor.h";
#include "./czujniki/Memory.h";
const int maxDevicesCount = 8;
Device * devices[maxDevicesCount];
int lastDeviceIndex = -1;

int znajdzCzujnik(String nazwa) {
  int indeks = -1;
  for (int i = 0; i <= lastDeviceIndex; i++)
    if (devices[i]->name == nazwa)
      indeks = i;
  return indeks;
}

#include "./silnik/Channel.h";
int channelsCount;
Channel * channels;

#include "./silnik/Surface.h";
int surfacesCount;
Surface * surfaces;

#include "./silnik/Transition.h";
Transition transition = Transition();

#include "./silnik/Program.h";
#include "./programy/Batterfly.h";
#include "./programy/Gravity.h";
#include "./programy/VV.h";
#include "./programy/Logo.h";
#include "./programy/Kontrolery.h";
#include "./programy/Skaner3d.h";
#include "./programy/Battery.h";
#include "./programy/Contacts.h";
const int maxProgramsCount = 8;
Program * programs[maxProgramsCount];
int activeProgram = -1;
int lastProgramIndex = -1;

#include "./silnik/Executor.h"
Executor executor;

#include "./silnik/Menu.h";
Menu menu;

void setup() {
  Serial.begin(9600);
  
  i2c.init();
  
  storage.init();
  const String path = "/config/surfaces/8.json";
  JsonArray & config = storage.load(path);

  channelsCount = Channel::count(config);
  channels = new Channel[channelsCount];
  for (int i = 0; i < channelsCount; i++)
    channels[i].init(i);

  Kontrolery * kontrolery = new Kontrolery();
  kontrolery->skanuj();
  
  for (int i = 0; i <= lastDeviceIndex; i++)
    devices[i]->init();

  surfacesCount = config.size();
  surfaces = new Surface[surfacesCount];
  for (int i = 0; i < surfacesCount; i++) {
    Surface * surface = new Surface();
    surface->init(config[i]);
    surfaces[i] = * surface;
  }

  menu.init();
}

void loop() {
  for (int i = 0; i <= lastDeviceIndex; i++)
    devices[i]->tick();

  for (int i = 0; i < channelsCount; i++)
    channels[i].tick();

  for (int i = 0; i < surfacesCount; i++)
    surfaces[i].tick();

  menu.tick();
  transition.tick();

  for (int i = 0; i <= lastProgramIndex; i++)
    programs[i]->tick();

  for (int i = 0; i < surfacesCount; i++)
    surfaces[i].clear();

  for (int i = 0; i <= lastProgramIndex; i++)
    programs[i]->draw();

  menu.draw();
  transition.draw();

  for (int i = 0; i < surfacesCount; i++)
    surfaces[i].draw();
}
