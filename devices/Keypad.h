class Keypad: public Device {
  public:
    KEYPAD device;
  
    void init() {
      this->device.begin();
      this->connected = true;//TODO: should take result from device.begin()
    }
  
    void tick() {
      this->device.updateFIFO();
    }

    boolean anyKeyPressed() {
      char button = this->device.getButton();
      return button != NULL;
    }

  Keypad() {}
};