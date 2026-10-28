#include "../czujniki/nachylenie.h";
#include "../czujniki/dzojstik.h";
#include "../czujniki/klawiatura-numeryczna.h";
#include "../czujniki/nfc.h";
#include "../czujniki/odleglosc.h";
#include "../czujniki/pamiec.h";

class port {
  public:
    int channel;
    int number;
    boolean hasDevices;
    czujnik * czujniki[2];
    OLED screen = OLED("ssd1306");
    
    void inicjuj() {
      i2c.activate(this->channel, this->number);
      this->czujniki[0] = new DistanceSensor(0x29);
      this->czujniki[0]->inicjuj();
      this->screen.init(128, 32, "ssd1306");
    }

    void display() {
      if (this->screen.connected) {
        if (this->screen.needsRefresh) {
          i2c.activate(this->channel, this->number);
          this->screen.tick();
        }
      }
    }

    void aktualizuj() {
      i2c.activate(this->channel, this->number);
      this->czujniki[0]->aktualizuj();
    }
    
  port() {}
};
