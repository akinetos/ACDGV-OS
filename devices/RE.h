class RE: public Device {
  public:
    uint16_t value = 0;
    int position = 0;
    boolean button = false;
    int buttonTimestamp = 0;
    boolean connected = false;
    boolean changed = false;
    
    int address = 0x54;
    DFRobot_VisualRotaryEncoder_I2C device = DFRobot_VisualRotaryEncoder_I2C(0x54, &Wire);
  
    void init() {
      this->device = DFRobot_VisualRotaryEncoder_I2C(this->address, &Wire);
      this->connected = NO_ERR == this->device.begin();
    }
  
    void tick() {
      this->button = false;
      this->changed = false;
      if (this->connected) {
        if (this->value != this->device.getEncoderValue()) {
          this->value = this->device.getEncoderValue();
          this->position = this->value / 51;
          this->changed = true;
        }
        if (this->device.detectButtonDown() && (millis() - this->buttonTimestamp > 1000)) {
          this->buttonTimestamp = millis();
          this->button = true;
          this->changed = true;
        }
      }
    }

  RE(int address) {
    this->address = address;
  }
};