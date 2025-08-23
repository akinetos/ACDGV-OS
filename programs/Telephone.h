class Telephone:public Program {
    public:
      String number;
      boolean contactsLoaded = false;

      void init() {
        this->number = "tel: ";
        this->initialised = true;
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
            s2x1.clear();
            p01.screen.ssd1306.setCursor(0, 0);
            p01.screen.textSize = 2;
            p01.screen.setTextSize();
            p01.screen.ssd1306.print(this->number);
            p01.screen.needsRefresh = true;
          }
        }
        
        if (this->option == 1) {
          if (!this->contactsLoaded) {
            JsonArray & file = interface.loadFromFile("/config/menu.json");
            if (s2x1.facingUp) {
              p01.screen.populate(file[1][5][1][1]);
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

    Telephone() {}
};
