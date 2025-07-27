class Telephone:public Program {
    public:
      String number;
      int option = 0;
      int startedTime;

      void init() {
        this->startedTime = millis();
        this->number = "tel: ";
      }

      void tick() {
        Surface & s2x1 = surfaces[0];
        Port & p01 = channels[s2x1.channel].ports[1];

        char button = keypad.device.getButton();
        if (button != 0) {
          this->number += button;
        }

        if (s2x1.facingUp) {
          p01.screen.clear();
          p01.screen.ssd1306.setCursor(0, 0);
          p01.screen.ssd1306.print(this->number);
          p01.screen.needsRefresh = true;
        }
      }

      void setOption(int option) {
        this->option = option;
      }

      void becameActive() {}

    Telephone() {}
};
