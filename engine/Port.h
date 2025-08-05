#include "../devices/OLED.h";

class Port {
  public:
    int channel;
    int number;
    
    OLED screen = OLED("ssd1306");
    AM accelerometer = AM(0x53);
    RE re = RE(0x54);
    
    void init(String mode) {
      if (mode == "devices") {
        i2c.activate(this->channel, this->number);
        this->accelerometer.init();
        this->re.init();
      }

      if (mode == "screens") {
        i2c.activate(this->channel, this->number);
        if (this->channel == 0 && this->number == 7) {
          this->screen.init(128, 64, "sh1106");
        } else {
          this->screen.init(128, 32, "ssd1306");
        }
      }
    }
    
    void tick(String mode) {
      if (mode == "devices") {
        if (this->accelerometer.connected) {
          i2c.activate(this->channel, this->number);
          this->accelerometer.tick();
        }
        if (this->re.connected) {
          i2c.activate(this->channel, this->number);
          this->re.tick();
        }
      }

      if (mode == "screens") {
        if (this->screen.connected) {
          if (this->screen.needsRefresh) {
            i2c.activate(this->channel, this->number);
            this->screen.tick();
          }
        }
      }
    }
    
  Port() {}
};
