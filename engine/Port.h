class Port {
  public:
    int channel;
    int number;
    boolean hasDevices;
    Device * devices[2];
    OLED screen = OLED("ssd1306");
    
    void init() {
      i2c.activate(this->channel, this->number);
      this->devices[0] = new DistanceSensor(0x29);
      this->devices[0]->init();
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

    void tick() {
      i2c.activate(this->channel, this->number);
      this->devices[0]->tick();
    }
    
  Port() {}
};
