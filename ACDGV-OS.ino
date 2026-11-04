#include "ArduinoJson.h"
#include "./silnik/Pixel.h";

#include "./silnik/katalog.h";

#include "./silnik/magistrala.h";
magistrala i2c;

#include "./czujniki/nachylenie.h";
#include "./czujniki/dzojstik.h";
#include "./czujniki/klawiatura-numeryczna.h";
#include "./czujniki/nfc.h";
#include "./czujniki/pamiec.h";

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

  katalog::inicjuj();
  
  const String sciezkaKonfiguracjiPrzestrzeni = "/config/surfaces/8.json";
  JsonArray & konfiguracjaPrzestrzeni = katalog::laduj(sciezkaKonfiguracjiPrzestrzeni);

  i2c.inicjuj();
  i2c.inicjujKanaly(konfiguracjaPrzestrzeni);

  kontrolery * wykrywaczCzujnikow = new kontrolery();
  wykrywaczCzujnikow->skanuj();
  
  i2c.zainicjujCzujniki();

  liczbaPrzestrzeni = konfiguracjaPrzestrzeni.size();
  przestrzenie = new przestrzen[liczbaPrzestrzeni];
  for (int i = 0; i < liczbaPrzestrzeni; i++)
    przestrzenie[i].inicjuj(konfiguracjaPrzestrzeni[i]);

  menu.inicjuj();
}


void loop() {
  i2c.aktualizuj();

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
