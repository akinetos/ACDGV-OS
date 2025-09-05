class I2c:public Program {
  public:
    String foundDevices[10];
    int count = 0;
    boolean scanned = false;

    JsonArray & loadFromFile (String filePath) {
      File file = SPIFFS.open(filePath, "r");
      if (file) {
        String source = file.readString();
        int sourceLength = source.length() + 1; 
        char charArray[sourceLength];
        source.toCharArray(charArray, sourceLength);
        StaticJsonBuffer<2250> jsonBuffer;
        JsonArray& data = jsonBuffer.parseArray(charArray);
        return data;
      }
    }

    void init() {
      if (!this->scanned) {
        this->scan();
      }
    }

    void scan() {
      JsonArray & configDevices = this->loadFromFile("/config/devices.json");
      for (int address = 8; address < 120; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
          String deviceType = "unknown";
          for (int d=0; d<configDevices.size(); d++) {
            for (int a=0; a<configDevices[d]["address"].size(); a++) {
              if (configDevices[d]["address"][a] == address) {
                String name = configDevices[d]["name"];
                deviceType = name;
              }
            }
          }
          foundDevices[count] = (String)address + " - " + deviceType;
          count++;
          delay(1);
        }
      }
      this->scanned = true;
    }

    void tick() {
      //OLED & screen = channels[0].ports[1].screen;
      if (this->scanned) {
        for (int i=0; i<this->count; i++) {
          //screen.lines[i] = this->foundDevices[i];
          channels[0].ports[i+1].screen.clear();
          channels[0].ports[i+1].screen.printText(this->foundDevices[i]);
        }
      } else {
        //screen.lines[0] = "not scanned";
      }
      //screen.hasOptions = true;
      //screen.optionsCount = this->count;
      //screen.minOffsetY = -(screen.optionsCount * 10) + screen.height - 1;
      //screen.lineSelected = -1;
      //screen.offsetY = 0;
      //if (gamepad.connected) {
      //  screen.updateScrollbar(gamepad.axisY);
      //}
      //screen.clear();
      //screen.printLines();
      //screen.drawScrollbar();
    }

  I2c() {}
};
