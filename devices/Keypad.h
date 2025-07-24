class Keypad: public Device {
  public:
    KEYPAD device;
  
    void init() {
      device.begin();
    }
  
    void tick() {
      this->device.updateFIFO();
    }

  Keypad() {}
};