class Telephone:public Program {
    public:
      String number;
      int option = 0;

      void init() {
        this->number = "";
      }

      void tick() {
        Surface & s2x1 = surfaces[0];
        Surface & s8x1 = surfaces[1];
        
        Port & p01 = channels[s2x1.channel].ports[1];

        Port & p10 = channels[s8x1.channel].ports[0];
        Port & p11 = channels[s8x1.channel].ports[1];

        char button = keypad.device.getButton();
        if (button != 0) {
          this->number += button;

          if (s2x1.facingUp) {
            p01.screen.clear();
            p01.screen.ssd1306.setCursor(0, 20);
            p01.screen.ssd1306.print(this->number);
            p01.screen.needsRefresh = true;
          }

          if (s8x1.facingUp) {
            p10.screen.clear();
            p10.screen.ssd1306.setCursor(0, 0);
            p10.screen.ssd1306.setTextSize(2);
            p10.screen.ssd1306.print("telefon");
            p10.screen.needsRefresh = true;

            p11.screen.clear();
            p11.screen.ssd1306.setCursor(0, 0);
            p11.screen.ssd1306.setTextSize(2);
            p11.screen.ssd1306.print(this->number);
            p11.screen.needsRefresh = true;
          }
        }
      }

      void setOption(int option) {
        this->option = option;
      }

      void becameActive() {}

    Telephone() {}
};
