class Telephone:public Program {
    public:
      boolean contactsLoaded = false;
      String contacts[8];

      void init() {
        this->initialised = true;
      }

      void tick() {
        if (this->contactsLoaded) {
          Surface * s8x1 = surfaces[0];
          if (s8x1->facingUp) {
            for (int i=0; i<8; i++) {
              channels[0].ports[i].screen.printText(contacts[i]);
            }
          }
        } else {
          JsonArray & menu = storage.load("/config/menu.json");
          for (int i=0; i<menu[1][1][1].size(); i++) {
            String name = menu[1][1][1][i][0];
            this->contacts[i] = name;
          }
          this->contactsLoaded = true;
        }
      }

    Telephone() {}
};
