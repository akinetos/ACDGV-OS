#include <Wire.h>
#include "./czujnik.h";
#include "./kanal.h";

class magistrala {
  public:
    int port = -1;
    int channel = -1;
    czujnik * czujniki[8];
    int indeksOstatniegoCzujnika = -1;
    int liczbaKanalow;
    kanal * kanaly;

    void inicjuj() {
      Wire.begin();
      Wire.setClock(400000);
    }

    void deactivate(int channel) {
      Wire.beginTransmission(112 + channel);
      Wire.write(0);
      Wire.endTransmission();
    }

    void activate(int channel, int port) {
      if (channel != this->channel || port != this->port) {
        if (channel != this->channel && this->channel >= 0) {
          this->deactivate(this->channel);
        }
        Wire.beginTransmission(112 + channel);
        Wire.write(1 << port);
        Wire.endTransmission();
        this->channel = channel;
        this->port = port;
      }
    }

    void writeTo(int device, byte address, byte val) {
      Wire.beginTransmission(device);
      Wire.write(address);
      Wire.write(val);
      Wire.endTransmission();
    }

    void readFrom(int device, byte address, int num, byte buff[]) {
      Wire.beginTransmission(device);
      Wire.write(address);
      Wire.endTransmission();
      Wire.requestFrom(device, num);
      int i = 0;
      while(Wire.available()) {
        buff[i] = Wire.read();
        i++;
      }
    }

    int znajdzCzujnik(String nazwa) {
      int indeks = -1;
      for (int i = 0; i <= indeksOstatniegoCzujnika; i++)
        if (this->czujniki[i]->name == nazwa)
          indeks = i;
      return indeks;
    }

    void zainicjujCzujniki() {
      for (int i = 0; i <= indeksOstatniegoCzujnika; i++)
        this->czujniki[i]->inicjuj();
    }

    void aktualizujCzujniki() {
      for (int i = 0; i <= indeksOstatniegoCzujnika; i++)
        this->czujniki[i]->aktualizuj();
    }

    void inicjujKanaly(JsonArray & konfiguracjaPrzestrzeni) {
      this->liczbaKanalow = kanal::count(konfiguracjaPrzestrzeni);
      this->kanaly = new kanal[liczbaKanalow];
      for (int k = 0; k < liczbaKanalow; k++) {
        this->kanaly[k].number = k;
        for (int p=0; p<8; p++) {
          this->kanaly[k].porty[p].number = p;
          this->kanaly[k].porty[p].channel = k;
          this->activate(k, p);
          this->kanaly[k].porty[p].inicjuj();
        }
        this->deactivate(k);
      }
    }

    void aktualizujKanaly() {
      for (int k = 0; k < liczbaKanalow; k++)
        for (int p=0; p<8; p++) {
          this->activate(k, p);
          this->kanaly[k].porty[p].aktualizuj();
        }
    }

    void aktualizuj() {
      this->aktualizujCzujniki();
      this->aktualizujKanaly();
    }

  magistrala() {}
};
