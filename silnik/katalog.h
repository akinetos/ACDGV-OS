#include <FS.h>

DynamicJsonBuffer jsonBuffer;

class katalog {
  public:
    static void inicjuj() {
      SPIFFS.begin();
    }

    static JsonArray & laduj (String sciezka) {
      File plik = SPIFFS.open(sciezka, "r");
      if (plik) {
        String trescPliku = plik.readString();
        jsonBuffer.clear();
        JsonArray & tablica = jsonBuffer.parseArray(trescPliku.c_str());
        return tablica;
      }
    }
};