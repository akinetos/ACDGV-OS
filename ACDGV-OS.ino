#include "ArduinoJson.h";
#include "silnik/piksel.h";

#include "silnik/katalog.h";

#include "silnik/magistrala.h";
magistrala i2c;

#include "silnik/przestrzen.h";
int liczbaPrzestrzeni;
przestrzen * przestrzenie;

#include "silnik/przejscie.h";
przejscie animacja;

#include "silnik/program.h";
const int maksymalnaLiczbaProgramow = 8;
program * programy[maksymalnaLiczbaProgramow];
int aktywnyProgram = -1;
int indeksOstatniegoProgramu = -1;

#include "wykonawca.h";
wykonawca agent;

#include "interfejs.h";
interfejs menu;

void setup() {
  Serial.begin(9600);

  katalog::inicjuj();
  
  const String sciezkaKonfiguracjiPrzestrzeni = "/config/surfaces/8.json";
  JsonArray & konfiguracjaPrzestrzeni = katalog::laduj(sciezkaKonfiguracjiPrzestrzeni);

  i2c.inicjuj(konfiguracjaPrzestrzeni);

  agent.inicjujCzujniki();

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
