#include "./ekran.h";

class port {
  public:
    int channel;
    int number;
    boolean hasDevices;
    czujnik * czujniki[2];
    ekran screen = ekran("ssd1306");
    
    void inicjuj() {
      this->screen.init(128, 32, "ssd1306");
    }

    void aktualizuj() {
      this->czujniki[0]->aktualizuj();
    }
    
  port() {}
};
