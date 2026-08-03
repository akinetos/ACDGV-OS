class DistanceSensor: public Device {
  public:
    int distance = 0;
    int previousDistance = 0;
  
    void init() {}
  
    void tick() {
      // Start single measurement
      Wire.beginTransmission(this->address);
      Wire.write(0x00); // SYSRANGE_START register
      Wire.write(0x01); // Data / start command
      Wire.endTransmission();

      // Read range result high and low byte from 0x1E
      Wire.beginTransmission(this->address);
      Wire.write(0x1E);
      Wire.endTransmission(false);
      
      Wire.requestFrom(this->address, 2);
      if (Wire.available() >= 2) {
        int highByte = Wire.read();
        int lowByte = Wire.read();
        this->distance = (highByte << 8) | lowByte;
      }
    }

    double readNumber(String parameter) {
      double output = 0;
      if (parameter == "distance") {
        output = (double)this->distance;
      }
      return output;
    }

  DistanceSensor(int address) {
    this->address = address;
  }
};