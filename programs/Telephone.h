class Telephone:public Program {
    public:
      String number;
      int option = 0;
      int startedTime;
      boolean contactsLoaded = false;

      void init() {
        this->startedTime = millis();
        this->number = "tel: ";
      }

      void tick() {
        Surface & s2x1 = surfaces[0];
        Port & p01 = channels[s2x1.channel].ports[1];

        if (this->option == 0) {
          char button = keypad.device.getButton();
          if (button != 0) {
            this->number += button;
          }
          if (s2x1.facingUp) {
            p01.screen.clear();
            p01.screen.ssd1306.setCursor(0, 0);
            p01.screen.ssd1306.print(wifi.connected ? "WiFi - connected" : "WiFi - not connected");
            p01.screen.ssd1306.setCursor(0, 10);
            p01.screen.ssd1306.print((String)wifi.attempts);
            p01.screen.ssd1306.setCursor(0, 20);
            p01.screen.ssd1306.print(this->number);
            p01.screen.needsRefresh = true;
          }
        }
        
        if (this->option == 1) {
          if (!this->contactsLoaded) {
            JsonArray & file = interface.loadFromFile("/menu.json");
            if (s2x1.facingUp) {
              s2x1.populateScreen(1, file[1][5][1][1]);
            }
            this->contactsLoaded = true;
          }
          if (s2x1.facingUp) {
            if (p01.screen.hasOptions) {
              p01.screen.clear();
              p01.screen.printBoxes();
              p01.screen.printLines();
              p01.screen.drawScrollbar();
            }
          }
        }
      }

      void setOption(int option) {
        this->option = option;
      }

      void becameActive() {}

    Telephone() {}
};
