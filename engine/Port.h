class Port {
  public:
    int channel;
    int number;
    boolean hasDevices;
    Device * devices[2];
    OLED screen = OLED("ssd1306");
    
    void init() {
      if (this->hasDevices) {
        /*
        this->devices[0] = new AM(83);
        this->devices[1] = new RE(0x54);
        for (int d = 0; d < 2; d++) {
          i2c.activate(this->channel, this->number);
          this->devices[d]->init();
        }
        */
      }
      i2c.activate(this->channel, this->number);
      this->screen.init(128, 32, "ssd1306");
    }
    
    void tick() {
      if (this->hasDevices) {
        /*
        for (int d = 0; d < 2; d++) {
          i2c.activate(this->channel, this->number);
          this->devices[d]->tick();
        }
        */
      }
    }

    void display() {
      if (this->screen.connected) {
        if (this->screen.needsRefresh) {
          i2c.activate(this->channel, this->number);
          this->screen.tick();
        }
      }
    }
    
  Port() {}
};
