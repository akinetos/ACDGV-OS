class I2c:public Program {
  public:
    String devices[10];
    int count = 0;
    boolean scanned = false;

    void init() {
      if (!this->scanned) {
        this->scan();
      }
    }

    void scan() {
      for (int i = 8; i < 120; i++) {
        Wire.beginTransmission(i);
        if (Wire.endTransmission() == 0) {
          String deviceType = "unknown";
          if (i == 81) {
            deviceType = "gamepad";
          }
          if (i == 75) {
            deviceType = "keypad";
          }
          if (i == 29 || i == 83) {
            deviceType = "accelerometer";
          }
          if (i == 112) {
            deviceType = "multiplexer";
          }
          if (i == 115) {
            deviceType = "gesture sensor";
          }
          devices[count] = (String)i + " - " + deviceType;
          count++;
          delay(1);
        }
      }
      this->scanned = true;
    }

    void tick() {
      OLED & screen = channels[0].ports[1].screen;
      if (this->scanned) {
        for (int i=0; i<this->count; i++) {
          screen.lines[i] = this->devices[i];
        }
      } else {
        screen.lines[0] = "not scanned";
      }
      screen.hasOptions = true;
      screen.optionsCount = this->count;
      screen.minOffsetY = -(screen.optionsCount * 10) + screen.height - 1;
      screen.lineSelected = -1;
      screen.offsetY = 0;
      if (gamepad.connected) {
        screen.updateScrollbar(gamepad.axisY);
      }
      screen.clear();
      screen.printLines();
      screen.drawScrollbar();
    }

  I2c() {}
};
