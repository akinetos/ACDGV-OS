class GV: public Device {
  public:
    DFRobot_MAX17043 device;
    double percentage = 0;
    double voltage = 0;
    String status = "";
    boolean changed = false;
    double previousPercentage = 0;
    int interval = 1000;
    int lastUpdated = millis();

    void init() {
      this->connected = this->device.begin() == 0;
    }
    
    void tick() {
      this->changed = false;
      if (this->connected) {
        if (millis() - this->lastUpdated > this->interval) {
          double percentage = this->device.readPercentage();
          double voltage = this->device.readVoltage();
          if (this->percentage != percentage || this->voltage != voltage) {
            this->percentage = percentage;
            this->voltage = voltage;
            this->changed = true;
          }
          if (percentage > this->previousPercentage) {
            this->status = "charging";
          } else {
            this->status = "discharging";
          }
          this->previousPercentage = percentage;
          this->lastUpdated = millis();
        }
      } else {
        this->init();
      }
    }

  GV() {}
};