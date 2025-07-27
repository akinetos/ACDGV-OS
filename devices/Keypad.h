class Keypad: public Device {
  public:
    KEYPAD device;
  
    void init() {
      this->device.begin();
    }
  
    void tick() {
      this->device.updateFIFO();
    }

  Keypad() {}
};