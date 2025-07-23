class I2C {
  public:
    int port = -1;
    int channel = -1;

    void init() {
      Wire.begin();
    }

    void deactivate(int channel) {
      Wire.beginTransmission(112 + channel);
      Wire.write(0);
      Wire.endTransmission();
    }

    void activate(int channel, int port) {
      if (channel != this->channel || port != this->port) {
        if (channel != this->channel && this->channel >= 0) {
          this->deactivate(this->channel);
        }
        Wire.beginTransmission(112 + channel);
        Wire.write(1 << port);
        Wire.endTransmission();
        this->channel = channel;
        this->port = port;
      }
    }

    void writeTo(int device, byte address, byte val) {
      Wire.beginTransmission(device);
      Wire.write(address);
      Wire.write(val);
      Wire.endTransmission();
    }

    void readFrom(int device, byte address, int num, byte buff[]) {
      Wire.beginTransmission(device);
      Wire.write(address);
      Wire.endTransmission();
      Wire.beginTransmission(device);
      Wire.requestFrom(device, num);
      int i = 0;
      while(Wire.available()) {
        buff[i] = Wire.read();
        i++;
      }
      Wire.endTransmission();
    }

  I2C() {}
};
