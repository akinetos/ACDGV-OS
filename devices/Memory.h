#include <Adafruit_EEPROM_I2C.h>

class Memory: public Device {
  public:
    Adafruit_EEPROM_I2C device;
    double number = 0;
    double number2 = 0;
  
    void init() {
      this->address = 80;
      int status = this->device.begin(this->address);
      this->connected = status == 1;
      if (this->connected) {
        double pi = 3.14159265351212121212;
        uint8_t buffer[16];
        memcpy(buffer, (void *)&pi, 16);
        this->device.write(0, buffer, 16);

        double phi = 1.61803398872323232323;
        uint8_t buffer2[16];
        memcpy(buffer2, (void *)&phi, 16);
        this->device.write(16, buffer2, 16);
      }
    }
  
    void tick() {
      if (this->connected) {
        if (this->number == 0) {
          uint8_t buffer[16];
          this->device.read(0, buffer, 16);
          memcpy((void *)&this->number, buffer, 16);

          uint8_t buffer2[16];
          this->device.read(16, buffer2, 16);
          memcpy((void *)&this->number2, buffer2, 16);
        } else {
          Serial.println(this->number, 16);
          Serial.println(this->number2, 16);
        }
      }
    }

  Memory() {}
};
