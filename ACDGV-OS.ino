#include "ArduinoJson.h"
#include "./silnik/katalog.h";

#include "./silnik/czujnik.h";
const int maksymalnaLiczbaCzujnikow = 8;
czujnik * czujniki[maksymalnaLiczbaCzujnikow];
int indeksOstatniegoCzujnika = -1;

#include "./silnik/magistrala.h";
magistrala i2c = magistrala();

#include "./czujniki/nachylenie.h";
#include "./czujniki/dzojstik.h";
#include "./czujniki/klawiatura-numeryczna.h";
#include "./czujniki/nfc.h";
#include "./czujniki/odleglosc.h";
#include "./czujniki/pamiec.h";

#include "./silnik/kanal.h";
int channelsCount;
kanal * kanaly;

#include "./silnik/przestrzen.h";
int liczbaPrzestrzeni;
przestrzen * przestrzenie;

#include "./silnik/przejscie.h";
przejscie transition = przejscie();

#include "./silnik/program.h";
#include "./programy/ptak.h";
#include "./programy/grawitacja.h";
#include "./programy/ww.h";
#include "./programy/logo.h";
#include "./programy/kontrolery.h";
#include "./programy/odleglosciomierz.h";
#include "./programy/bateria.h";
#include "./programy/kontakty.h";
const int maksymalnaLiczbaProgramow = 8;
program * programy[maksymalnaLiczbaProgramow];
int aktywnyProgram = -1;
int indeksOstatniegoProgramu = -1;

#include "./silnik/wykonawca.h"
wykonawca agent;

#include "./silnik/menu.h";
Menu menu;

void setup() {
  Serial.begin(9600);
  
  i2c.init();
  
  katalog::init();
  const String path = "/config/surfaces/8.json";
  JsonArray & config = katalog::load(path);

  channelsCount = kanal::count(config);
  kanaly = new kanal[channelsCount];
  for (int i = 0; i < channelsCount; i++)
    kanaly[i].init(i);

  Kontrolery * kontrolery = new Kontrolery();
  kontrolery->skanuj();
  
  for (int i = 0; i <= indeksOstatniegoCzujnika; i++)
    czujniki[i]->init();

  liczbaPrzestrzeni = config.size();
  przestrzenie = new przestrzen[liczbaPrzestrzeni];
  for (int i = 0; i < liczbaPrzestrzeni; i++)
    przestrzenie[i].init(config[i]);

  menu.init();
}

void loop() {
  for (int i = 0; i <= indeksOstatniegoCzujnika; i++)
    czujniki[i]->tick();

  for (int i = 0; i < channelsCount; i++)
    kanaly[i].tick();

  for (int i = 0; i < liczbaPrzestrzeni; i++)
    przestrzenie[i].tick();

  menu.tick();
  transition.tick();

  for (int i = 0; i <= indeksOstatniegoProgramu; i++)
    programy[i]->tick();

  for (int i = 0; i < liczbaPrzestrzeni; i++)
    przestrzenie[i].clear();

  for (int i = 0; i <= indeksOstatniegoProgramu; i++)
    programy[i]->draw();

  menu.draw();
  transition.draw();

  for (int i = 0; i < liczbaPrzestrzeni; i++)
    przestrzenie[i].draw();
}
