class Interface:public Program {
  public:
    String levels[8];

    String getBreadcrumbs() {
      String output = "";
      for (int i=0; i<8; i++) {
        if (this->levels[i] != "") {
          if (output != "") {
            output += "/";
          }
          output += levels[i];
        }
      }
      return output;
    }

    void updateContent() {
      Surface & surface = surfaces[0];
      int x = surface.getRelativeX();

      if (surface.pointerPortChanged()) {
        OLED & previousScreen = channels[surface.channel].ports[surface.pointerPreviousPort].screen;
        previousScreen.resetLineHovered();
      }

      OLED & screen1 = channels[surface.channel].ports[1].screen;
      screen1.backButtonHovered = x < 10;
      screen1.scrollbarHovered = x > (screen1.width-10);
      if (screen1.scrollbarHovered) {
        screen1.updateScrollbar(accelerometer.y);
      } else {
        int y = surface.getRelativeY();
        screen1.lineHovered = (int)((y - screen1.offsetY) / 10);
        if (screen1.lineHoveredChanged()) {
          screen1.previousLineHovered = screen1.lineHovered;
        }
      }
    }

    void updatePointer() {
      Surface & surface = surfaces[0];
      if (surface.pointerPositionX > 120) {
        surface.pointerPositionY = surface.pointerPreviousPositionY;
        if (surface.pointerPortChanged()) {
          surface.pointerPort = surface.pointerPreviousPort;
        }
      }
    }

    JsonArray & loadFromFile (String filePath) {
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

    void init() {
      Surface & s2x1 = surfaces[0];
      JsonArray & file = this->loadFromFile("/menu.json");
      String root = file[0];
      s2x1.populateScreen(1, file);
      for (int i=0; i<8; i++) {
        this->levels[i] = "";
      }
      this->levels[0] = root;
      if (activeProgram > -1) {
        this->setProgram(activeProgram);
      }
    }

    int getMenuLevel() {
      int output = 0;
      for (int i=0; i<8; i++) {
        if (this->levels[i] != "") {
          output = i;
        }
      }
      return output;
    }

    void setProgram(int index) {
      OLED & screen1 = channels[0].ports[1].screen;
      screen1.lineSelected = index;
      activeProgram = index;
      JsonArray & file = this->loadFromFile("/menu.json");
      surfaces[0].populateScreen(1, file[1][activeProgram]);
      surfaces[1].clear();
      String programName = file[1][activeProgram][0];
      this->levels[1] = programName;
    }

    void populateOptions() {
      Surface & s2x1 = surfaces[0];
      int newMenuLevel = this->getMenuLevel();
      JsonArray & file = this->loadFromFile("/menu.json");
      if (newMenuLevel == 1) {
        s2x1.populateScreen(1, file[1][activeProgram]);
      }
      if (newMenuLevel == 0) {
        s2x1.populateScreen(1, file);
      }
    }

    void tick() {
      Surface & s2x1 = surfaces[0];
      Surface & s8x1 = surfaces[1];

      if (s2x1.facingUp) {
        Port & p05 = channels[s2x1.channel].ports[5];
        OLED & screen0 = channels[s2x1.channel].ports[0].screen;
        OLED & screen1 = channels[s2x1.channel].ports[1].screen;

        this->updatePointer();
        this->updateContent();
        
        String breadcrumbs = this->getBreadcrumbs();
        screen0.clear();

        int cursorX = -1;
        if (s2x1.pointerPort == 0) {
          cursorX = s2x1.getRelativeX();
        }
        screen0.drawBreadcrumbs(breadcrumbs, cursorX);
        int menuLevel = this->getMenuLevel();
        if (screen1.hasOptions) {
          screen1.clear();
          s2x1.drawBackButton(1);
          screen1.printLines();
          screen1.drawScrollbar();
        }
        
        s2x1.drawPointer();

        if (gamepad.buttonApressed()) {
          if (s2x1.pointerPort == 0) {
            if (screen0.breadcrumbHovered > -1) {
              for (int i=0; i < 8; i++) {
                if (i > screen0.breadcrumbHovered) {
                  this->levels[i] = "";
                }
              }
              this->populateOptions();
            }
          }

          if (s2x1.pointerPort == 1) {
            if (screen1.backButtonHovered) {
              int menuLevel = this->getMenuLevel();
              if (menuLevel > 0) {
                this->levels[menuLevel] = "";
                this->populateOptions();
              }
            } else {
              int index = screen1.lineHovered;
              if (menuLevel == 0) {
                this->setProgram(index);
              }
              if (menuLevel == 1) {
                JsonArray & file = this->loadFromFile("/menu.json");
                s2x1.populateScreen(1, file[1][activeProgram][1][index]);
                String programOption = file[1][activeProgram][1][index][0];
                this->levels[2] = programOption;
                programs[activeProgram]->setOption(index);
                programs[activeProgram]->becameActive();
              }
            }
          }
        }
      }

      if (s8x1.facingUp) {
        if (activeProgram == -1) {
          OLED & screen = channels[s8x1.channel].ports[0].screen;
          screen.clear();
          screen.printText("no program selected");
        }
      }
    }

    Interface() {}
};