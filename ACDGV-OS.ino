#define GLOSNIK_1 14

unsigned int oktawa4[] = {
  262, // C4
  294, // D4
  330, // E4
  349, // F4
  392, // G4
  440, // A4
  494, // B4
  523  // C4
};

// stałe czasowe w milisekundach
const unsigned long nuta = 400;
const unsigned long przerwaMiedzyNutami = 50;
const unsigned long przerwaMiedzySkalami = 2000;

// kontrola aktualnego stanu
unsigned long aktualnyCzas = 0;
unsigned long poprzedniCzas = 0;
int indeksNuty = 0;
bool graNute = false;
bool czekaNaPowtorzenieSkali = false;

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

  pinMode(GLOSNIK_1, OUTPUT);

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
  aktualnyCzas = millis();
  if (czekaNaPowtorzenieSkali) {
    if (aktualnyCzas - poprzedniCzas >= przerwaMiedzySkalami) {
      czekaNaPowtorzenieSkali = false;
      indeksNuty = 0;
      tone(GLOSNIK_1, oktawa4[indeksNuty]);
      graNute = true;
      poprzedniCzas = aktualnyCzas;
    }
  } else if (graNute) {
    if (aktualnyCzas - poprzedniCzas >= nuta) {
      noTone(GLOSNIK_1);
      graNute = false;
      poprzedniCzas = aktualnyCzas;
    }
  } else {
    if (aktualnyCzas - poprzedniCzas >= przerwaMiedzyNutami) {
      indeksNuty++;
      if (indeksNuty >= 8) {
        czekaNaPowtorzenieSkali = true;
      } else {
        tone(GLOSNIK_1, oktawa4[indeksNuty]);
        graNute = true;
      }
      poprzedniCzas = aktualnyCzas;
    }
  }

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
