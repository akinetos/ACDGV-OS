class Memory: public Device {
  public:
    Adafruit_EEPROM_I2C device;
    double number = 0;
  
    void init() {
      int status = this->device.begin(0x50);
      this->connected = status == 1;
      if (this->connected) {
        double pi = 3.1415926535897932384626433832795;
        uint8_t buffer[8];
        memcpy(buffer, (void *)&pi, 8);
        this->device.write(0x00, buffer, 8);
      }
    }
  
    void tick() {
      if (this->connected) {
        if (this->number == 0) {
          uint8_t buffer[8];
          this->device.read(0x00, buffer, 8);
          memcpy((void *)&this->number, buffer, 8);
        } else {
          Serial.println(this->number, 16);
        }
      }
    }

  Memory() {}
};
