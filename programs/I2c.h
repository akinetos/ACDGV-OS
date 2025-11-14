class I2c:public Program {
  public:
    String foundDevices[10];
    int foundAddresses[10];
    int count = 0;
    boolean scanned = false;

    void init() {
      if (!this->scanned) {
        this->scan();
      }
    }

    void scan() {
      JsonArray & controllers = loadFromFile("/config/sensors.json");
      for (int address = 8; address < 120; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
          String deviceType = "unknown";
          for (int d=0; d<controllers.size(); d++) {
            for (int a=0; a<controllers[d]["address"].size(); a++) {
              if (controllers[d]["address"][a] == address) {
                String name = controllers[d]["name"];
                deviceType = name;
              }
            }
          }
          foundAddresses[count] = address;
          foundDevices[count] = deviceType;
          count++;
          delay(1);
        }
      }
      this->scanned = true;
    }

    void tick() {
      if (this->scanned) {
        for (int i=0; i<this->count; i++) {
          OLED & screen = channels[0].ports[i+1].screen;
          const int amount = 3;

          screen.hasOptions = amount > 0;
          screen.optionsCount = amount;
          screen.minOffsetY = -(screen.optionsCount * 10) + screen.height - 1;
          screen.lines[0] = String(i+1);
          screen.lines[1] = this->foundAddresses[i];
          screen.lines[2] = this->foundDevices[i];
          screen.lineSelected = -1;
          for (int i=0; i<=20; i++) {
            screen.lineScrollWidth[i] = 0;
          }
          screen.offsetY = 0;
          screen.printLines();
        }
      } else {
        OLED & screen = channels[0].ports[1].screen;
        screen.hasOptions = true;
        screen.lines[0] = "Scanning I2C";
        screen.lines[1] = "...";
        screen.printLines();
      }
    }

  I2c() {}
};
