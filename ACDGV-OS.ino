#include "ArduinoJson.h"

#include "./silnik/katalog.h";

#include "./silnik/czujnik.h";
const int maksymalnaLiczbaCzujnikow = 8;
czujnik * czujniki[maksymalnaLiczbaCzujnikow];
int indeksOstatniegoCzujnika = -1;

#include "./silnik/magistrala.h";
magistrala i2c;

#include "./silnik/kanal.h";
int liczbaKanalow;
kanal * kanaly;

#include "./silnik/przestrzen.h";
int liczbaPrzestrzeni;
przestrzen * przestrzenie;

#include "./silnik/przejscie.h";
przejscie animacja;

#include "./silnik/program.h";
const int maksymalnaLiczbaProgramow = 8;
program * programy[maksymalnaLiczbaProgramow];
int aktywnyProgram = -1;
int indeksOstatniegoProgramu = -1;

#include "./silnik/wykonawca.h"
wykonawca agent;

#include "./silnik/interfejs.h";
interfejs menu;

void setup() {
  Serial.begin(9600);
  
  i2c.inicjuj();
  
  katalog::inicjuj();
  
  const String sciezkaKonfiguracjiPrzestrzeni = "/config/surfaces/8.json";
  JsonArray & konfiguracjaPrzestrzeni = katalog::load(sciezkaKonfiguracjiPrzestrzeni);

  liczbaKanalow = kanal::count(konfiguracjaPrzestrzeni);
  kanaly = new kanal[liczbaKanalow];
  for (int i = 0; i < liczbaKanalow; i++)
    kanaly[i].inicjuj(i);

  kontrolery * wykrywaczCzujnikow = new kontrolery();
  wykrywaczCzujnikow->skanuj();
  
  for (int i = 0; i <= indeksOstatniegoCzujnika; i++)
    czujniki[i]->inicjuj();

  liczbaPrzestrzeni = konfiguracjaPrzestrzeni.size();
  przestrzenie = new przestrzen[liczbaPrzestrzeni];
  for (int i = 0; i < liczbaPrzestrzeni; i++)
    przestrzenie[i].inicjuj(konfiguracjaPrzestrzeni[i]);

  menu.inicjuj();
}

void loop() {
  for (int i = 0; i <= indeksOstatniegoCzujnika; i++)
    czujniki[i]->aktualizuj();

  for (int i = 0; i < liczbaKanalow; i++)
    kanaly[i].aktualizuj();

  for (int i = 0; i < liczbaPrzestrzeni; i++)
    przestrzenie[i].aktualizuj();

  menu.aktualizuj();
  animacja.aktualizuj();

  for (int i = 0; i <= indeksOstatniegoProgramu; i++)
    programy[i]->aktualizuj();

  for (int i = 0; i < liczbaPrzestrzeni; i++)
    przestrzenie[i].czysc();

  for (int i = 0; i <= indeksOstatniegoProgramu; i++)
    programy[i]->rysuj();

  menu.rysuj();
  animacja.rysuj();

  for (int i = 0; i < liczbaPrzestrzeni; i++)
    przestrzenie[i].rysuj();
}