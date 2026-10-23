#include "ArduinoJson.h"
#include "./silnik/katalog.h";

#include "./silnik/czujnik.h";
const int maksymalnaLiczbaCzujnikow = 8;
czujnik * czujniki[maksymalnaLiczbaCzujnikow];
int indeksOstatniegoCzujnika = -1;

#include "./silnik/magistrala.h";
magistrala i2c;

#include "./czujniki/nachylenie.h";
#include "./czujniki/dzojstik.h";
#include "./czujniki/klawiatura-numeryczna.h";
#include "./czujniki/nfc.h";
#include "./czujniki/odleglosc.h";
#include "./czujniki/pamiec.h";

#include "./silnik/kanal.h";
int liczbaKanalow;
kanal * kanaly;

#include "./silnik/przestrzen.h";
int liczbaPrzestrzeni;
przestrzen * przestrzenie;

#include "./silnik/przejscie.h";
przejscie animacja;

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
  const String sciezkaKonfiguracjiPrzestrzeni = "/config/surfaces/8.json";
  JsonArray & konfiguracjaPrzestrzeni = katalog::load(sciezkaKonfiguracjiPrzestrzeni);

  liczbaKanalow = kanal::count(konfiguracjaPrzestrzeni);
  kanaly = new kanal[liczbaKanalow];
  for (int i = 0; i < liczbaKanalow; i++)
    kanaly[i].init(i);

  Kontrolery * kontrolery = new Kontrolery();
  kontrolery->skanuj();
  
  for (int i = 0; i <= indeksOstatniegoCzujnika; i++)
    czujniki[i]->init();

  liczbaPrzestrzeni = konfiguracjaPrzestrzeni.size();
  przestrzenie = new przestrzen[liczbaPrzestrzeni];
  for (int i = 0; i < liczbaPrzestrzeni; i++)
    przestrzenie[i].init(konfiguracjaPrzestrzeni[i]);

  menu.init();
}

void loop() {
  for (int i = 0; i <= indeksOstatniegoCzujnika; i++)
    czujniki[i]->tick();

  for (int i = 0; i < liczbaKanalow; i++)
    kanaly[i].tick();

  for (int i = 0; i < liczbaPrzestrzeni; i++)
    przestrzenie[i].tick();

  menu.tick();
  animacja.tick();

  for (int i = 0; i <= indeksOstatniegoProgramu; i++)
    programy[i]->tick();

  for (int i = 0; i < liczbaPrzestrzeni; i++)
    przestrzenie[i].clear();

  for (int i = 0; i <= indeksOstatniegoProgramu; i++)
    programy[i]->draw();

  menu.draw();
  animacja.draw();

  for (int i = 0; i < liczbaPrzestrzeni; i++)
    przestrzenie[i].draw();
}
