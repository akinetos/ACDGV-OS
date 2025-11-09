class HRS: public Device {
  public:
    DFRobot_BloodOxygen_S_I2C device = DFRobot_BloodOxygen_S_I2C(&Wire, 0x57);
    int previousRate;
    int rate;
    double previousTemperature;
    double temperature;
    boolean changed = false;
    int lastTick = 0;
    int connectionAttempts = 0;

    void init() {
      this->device = DFRobot_BloodOxygen_S_I2C(&Wire, this->address);
      this->connected = this->device.begin();
      this->connectionAttempts++;
      if (this->connected) {
        this->device.sensorStartCollect();
      }
    }
    
    void tick() {
      if (this->connected) {
        if (millis() - this->lastTick > 500) {
          this->device.getHeartbeatSPO2();
          this->rate = this->device._sHeartbeatSPO2.Heartbeat;
          this->temperature = this->device.getTemperature_C();
          this->changed = this->rate != this->previousRate || this->temperature != this->previousTemperature;
          this->lastTick = millis();
        }
      } else {
        if (this->connectionAttempts < 10) {
          this->init();
        }
      }
    }

  HRS() {}

  HRS(int address) { //0x57
    this->address = address;
  }
};
