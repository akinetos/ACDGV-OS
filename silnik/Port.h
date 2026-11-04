#include "./OLED.h";
#include "../czujniki/odleglosc.h";

class port {
  public:
    int channel;
    int number;
    boolean hasDevices;
    czujnik * czujniki[2];
    OLED screen = OLED("ssd1306");
    
    void inicjuj() {
      this->czujniki[0] = new DistanceSensor(0x29);
      this->czujniki[0]->inicjuj();
      this->screen.init(128, 32, "ssd1306");
    }

    void aktualizuj() {
      this->czujniki[0]->aktualizuj();
    }
    
  port() {}
};
