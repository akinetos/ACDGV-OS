class Menu:public Program {
    public:
        int surfaceIndex = -1;
        boolean displayed = false;

    void updateOptions() {
      Surface & surface = surfaces[this->surfaceIndex];

      if (surface.pointerPortChanged()) {
        OLED & previousScreen = channels[surface.channel].ports[surface.pointerPreviousPort].screen;
        previousScreen.lineHovered = -1;
        previousScreen.lineHoveredTimestamp = 0;
        previousScreen.needsRefresh = true;
      }

      OLED & screen = channels[surface.channel].ports[surface.pointerPort].screen;
      int x = surface.getRelativeX();
      int y = surface.getRelativeY();
      if (x > screen.width*0.38 && x < (screen.width-10)) {
        screen.lineHovered = (int)((y - screen.offsetY) / 10);
        if (screen.lineHoveredChanged()) {
          screen.lineHoveredTimestamp = millis();
          screen.previousLineHovered = screen.lineHovered;
        } else {
          if (millis() - screen.lineHoveredTimestamp > 2000) {
            /*
            screen.lineHoveredTimestamp = 0;
            String filePath = "/menu/" + (String)(this->surfaceIndex) + "/.json";
            JsonArray& menu = loadFromFile(filePath);
            JsonArray& branch = menu[1][surface.pointerPort][1][screen.lineHovered];
            this->display(branch);
            */
          }
        }
      }
      
      if (surface.pointerPositionX > 120) {
        screen.scrollbarHovered = true;
        screen.offsetY -= (int)(accelerometer.y * 20);
        if (screen.offsetY > 0) {
            screen.offsetY = 0;
        }
        if (screen.offsetY < -210) {
            screen.offsetY = -210;
        }
        screen.needsRefresh = true;
      } else {
        screen.scrollbarHovered = false;
      }
    }

    void updatePointerPosition() {
      Surface & surface = surfaces[this->surfaceIndex];
      if (surface.pointerPositionX > 120) {
        surface.pointerPositionY = surface.pointerPreviousPositionY;
      }
    }

    void display (JsonArray& menu) {
      this->displayed = false;
      const int size = menu[1].size();
      const int channelNumber = surfaces[this->surfaceIndex].channel;
      for (int p=0; p<8; p++) {
        OLED & screen = channels[channelNumber].ports[p].screen;
        if (p<size) {
          String branchName = menu[1][p][0];
          const int amount = menu[1][p][1].size();
          screen.heading = branchName;
          screen.hasOptions = amount > 0;
          for (int i = 0; i < amount; i++) {
            String optionName = menu[1][p][1][i][0];
            screen.lines[i] = optionName;
          }
        } else {
          screen.heading = "";
          screen.hasOptions = false;
        }
        screen.needsRefresh = true;
      }
    }

    JsonArray& loadFromFile (String filePath) {
      File menuFile = SPIFFS.open(filePath, "r");
      if (menuFile) {
        String source = menuFile.readString();
        int sourceLength = source.length() + 1; 
        char charArray[sourceLength];
        source.toCharArray(charArray, sourceLength);
        StaticJsonBuffer<2000> jsonBuffer;
        JsonArray& menu = jsonBuffer.parseArray(charArray);
        return menu;
      }
    }
    
    void drawOptions() {
      Surface & surface = surfaces[this->surfaceIndex];
      for (int p=0; p<8; p++) {
        OLED & screen = channels[surface.channel].ports[p].screen;
        if (
          !this->displayed ||
          p == surface.pointerPort || 
          screen.hasLongLines() ||
          (surface.pointerPortChanged() && p == surface.pointerPreviousPort)
        ) {
          screen.clear();
          screen.printLines();
          screen.printHeading();
          screen.drawScrollbar();
          screen.needsRefresh = true;
        }
      }
      this->displayed = true;
    }

    void drawPointer() {
      Surface & surface = surfaces[this->surfaceIndex];
      OLED & screen = channels[surface.channel].ports[surface.pointerPort].screen;
      screen.drawPointer(surface.getRelativeX(), surface.getRelativeY(), "circle");
    }

    void init() {
      this->surfaceIndex = 1;
      String filePath = "/menu/" + (String)(this->surfaceIndex) + "/.json";
      JsonArray& menu = loadFromFile(filePath);
      this->display(menu);
    }

    void tick() {
      Surface & surface = surfaces[this->surfaceIndex];
      if (surface.facingUp) {
        this->updatePointerPosition();
        this->updateOptions();
        this->drawOptions();
        this->drawPointer();
      } else {
        channels[0].ports[1].screen.heading = "DRZ.";
        channels[0].ports[1].screen.lines[0] = "zero";
        channels[0].ports[1].screen.lines[1] = "jeden";
        channels[0].ports[1].screen.lines[2] = "dwa";
        channels[0].ports[1].screen.hasOptions = true;
        channels[0].ports[1].screen.needsRefresh = true;
      }
    }

    void becameActive() {}

    Menu() {}
};
