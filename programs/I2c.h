class item {
  public:
    String name;
    int address[8];
  item() {}
};

class I2c:public Program {
  public:
    String foundDevices[10];
    int foundAddresses[10];
    int count = 0;
    int knownDevicesCount = 0;
    item list[10];

    void init() {
      if (this->knownDevicesCount == 0) {
        this->prepare();
      }
      this->scan();
    }

    void prepare() {
      this->list[0].name = "multiplexer";
      this->list[0].address[0] = 112;
      this->list[0].address[1] = 113;
      this->list[0].address[2] = 114;
      this->list[1].name = "OLED";
      this->list[1].address[0] = 60;
      this->knownDevicesCount = 2;
      JsonArray & sensors = storage.load("/config/sensors.json");
      int sensorsCount = sensors.size();
      for (int d=0; d<sensorsCount; d++) {
        int index = this->knownDevicesCount;
        String name = sensors[d]["name"];
        this->list[index].name = name;
        for (int a=0; a<sensors[d]["address"].size(); a++) {
          int address = sensors[d]["address"][a];
          this->list[index].address[a] = address;
        }
        this->knownDevicesCount++;
      }
    }

    void scan() {
      this->count = 0;
      for (int address = 8; address < 120; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
          String deviceType = "unknown";
          for (int index=0; index<this->knownDevicesCount; index++) {
            for (int a=0; a<8; a++) {
              if (this->list[index].address[a] == address) {
                String name = this->list[index].name;
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
    }

    void tick() {
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
    }

  I2c() {}
};
