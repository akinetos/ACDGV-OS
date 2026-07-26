class DistanceSensor: public Device {
  public:
    Adafruit_VL53L0X device;
    int distance = 0;
    int previousDistance = 0;
    boolean polaczone = false;
  
    void init() {
      this->device = Adafruit_VL53L0X();
      //this->device.begin();
    }
  
    void tick() {
      /*
      this->changed = false;
      this->previousDistance = this->distance;
      VL53L0X_RangingMeasurementData_t measure;
      this->device.rangingTest(&measure, false);
      if (measure.RangeStatus != 4) {
        this->distance = measure.RangeMilliMeter;
        if (this->distance != this->previousDistance) {
          this->changed = true;
          Serial.println(this->distance);
        }
      }
      */
    }

    double readNumber(String parameter) {
      double output = 0;
      if (parameter == "distance") {
        output = (double)this->distance;
      }
      return output;
    }

  DistanceSensor() {}
};