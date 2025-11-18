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
    boolean scanned = false;
    String stats = "";
    item list[10];
    int totalDevicesCount = 0;

    void init() {
      if (!this->scanned) {
        this->scan();
      }
    }

    void scan() {
      JsonArray & devices = loadFromFile("/config/devices.json");
      int devicesCount = devices.size();
      for (int d=0; d<devicesCount; d++) {
        String name = devices[d]["name"];
        this->list[d].name = name;
        for (int a=0; a<devices[d]["address"].size(); a++) {
          int address = devices[d]["address"][a];
          this->list[d].address[a] = address;
        }
      }

      JsonArray & sensors = loadFromFile("/config/sensors.json");
      int sensorsCount = sensors.size();
      for (int d=0; d<sensorsCount; d++) {
        int index = d + devicesCount;
        String name = sensors[d]["name"];
        this->list[index].name = name;
        for (int a=0; a<sensors[d]["address"].size(); a++) {
          int address = sensors[d]["address"][a];
          this->list[index].address[a] = address;
        }
      }

      this->totalDevicesCount = devicesCount + sensorsCount;

      for (int address = 8; address < 120; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
          String deviceType = "unknown";
          
          for (int d=0; d<(devicesCount+sensorsCount); d++) {
            for (int a=0; a<8; a++) {
              if (this->list[d].address[a] == address) {
                String name = this->list[d].name;
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
        OLED & statsScreen = channels[0].ports[7].screen;
        statsScreen.hasOptions = true;
        statsScreen.optionsCount = 1;
        statsScreen.lines[0] = (String)this->totalDevicesCount;
        statsScreen.lineSelected = -1;
        statsScreen.printLines();
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
