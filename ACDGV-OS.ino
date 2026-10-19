#include "ArduinoJson.h"

#include "./silnik/i2c.h";
I2C i2c = I2C();

#include "./silnik/pliki.h";
Storage storage = Storage();

#include "./silnik/czujnik.h";
#include "./czujniki/nachylenie.h";
#include "./czujniki/dzojstik.h";
#include "./czujniki/klawiatura-numeryczna.h";
#include "./czujniki/nfc.h";
#include "./czujniki/odleglosc.h";
#include "./czujniki/pamiec.h";
const int maxDevicesCount = 8;
czujnik * czujniki[maxDevicesCount];
int lastDeviceIndex = -1;

int znajdzCzujnik(String nazwa) {
  int indeks = -1;
  for (int i = 0; i <= lastDeviceIndex; i++)
    if (czujniki[i]->name == nazwa)
      indeks = i;
  return indeks;
}

#include "./silnik/kanal.h";
int channelsCount;
Channel * channels;

#include "./silnik/przestrzen.h";
int surfacesCount;
przestrzen * przestrzenie;

#include "./silnik/przejscie.h";
Transition transition = Transition();

#include "./silnik/program.h";
#include "./programy/ptak.h";
#include "./programy/grawitacja.h";
#include "./programy/ww.h";
#include "./programy/logo.h";
#include "./programy/kontrolery.h";
#include "./programy/odleglosciomierz.h";
#include "./programy/bateria.h";
#include "./programy/kontakty.h";
const int maxProgramsCount = 8;
Program * programs[maxProgramsCount];
int activeProgram = -1;
int lastProgramIndex = -1;

#include "./silnik/wykonawca.h"
Executor executor;

#include "./silnik/menu.h";
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
    czujniki[i]->init();

  surfacesCount = config.size();
  przestrzenie = new przestrzen[surfacesCount];
  for (int i = 0; i < surfacesCount; i++)
    przestrzenie[i].init(config[i]);

  menu.init();
}

void loop() {
  for (int i = 0; i <= lastDeviceIndex; i++)
    czujniki[i]->tick();

  for (int i = 0; i < channelsCount; i++)
    channels[i].tick();

  for (int i = 0; i < surfacesCount; i++)
    przestrzenie[i].tick();

  menu.tick();
  transition.tick();

  for (int i = 0; i <= lastProgramIndex; i++)
    programs[i]->tick();

  for (int i = 0; i < surfacesCount; i++)
    przestrzenie[i].clear();

  for (int i = 0; i <= lastProgramIndex; i++)
    programs[i]->draw();

  menu.draw();
  transition.draw();

  for (int i = 0; i < surfacesCount; i++)
    przestrzenie[i].draw();
}
