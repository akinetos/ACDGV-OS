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
      for (byte i = 8; i < 120; i++) {
        Wire.beginTransmission(i);
        if (Wire.endTransmission() == 0) {
          String bitsString = "";
          for (int b=0; b<8; b++) {
            bitsString += bitRead(i, b);
          }
          devices[count] = bitsString + " " + (String)i;
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
      screen.clear();
      screen.printBoxes();
      screen.printLines();
      screen.drawScrollbar();
    }

  I2c() {}
};
