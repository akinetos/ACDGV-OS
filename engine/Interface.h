class Interface:public Program {
  public:
    String levels[8];
    int menuLevel = 0;

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

      OLED & previewScreen0 = channels[surface.channel].ports[0].screen;
      OLED & previewScreen1 = channels[surface.channel].ports[1].screen;
      if (surface.pointerPort == 0) {
        previewScreen0.backButtonHovered = x < 10;
      }

      if (surface.pointerPort == 1) {
        previewScreen1.scrollbarHovered = x > (previewScreen1.width-10);
        if (previewScreen1.scrollbarHovered) {
          previewScreen1.updateScrollbar(gamepad.axisY);
        } else {
          int y = surface.getRelativeY();
          if (surface.pointerPositionX < 118) {
            previewScreen1.lineHovered = (int)((y - previewScreen1.offsetY) / 10);
          } else {
            previewScreen1.lineHovered = -1;
          }
          if (previewScreen1.lineHoveredChanged()) {
            previewScreen1.previousLineHovered = previewScreen1.lineHovered;
          }
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
      Surface & previewSurface = surfaces[0];
      JsonArray & file = this->loadFromFile("/menu.json");
      String root = file[0];
      previewSurface.populateScreen(1, file);
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
      OLED & previewScreen1 = channels[0].ports[1].screen;
      previewScreen1.lineSelected = index;
      if (programs[index]->startedTime == 0) {
        programs[index]->init();
      }
      activeProgram = index;
      JsonArray & file = this->loadFromFile("/menu.json");
      surfaces[0].populateScreen(1, file[1][activeProgram]);
      surfaces[1].clear();
      String programName = file[1][activeProgram][0];
      this->levels[1] = programName;
    }

    void populateOptions() {
      Surface & previewSurface = surfaces[0];
      int newMenuLevel = this->getMenuLevel();
      JsonArray & file = this->loadFromFile("/menu.json");
      if (newMenuLevel == 1) {
        previewSurface.populateScreen(1, file[1][activeProgram]);
      }
      if (newMenuLevel == 0) {
        previewSurface.populateScreen(1, file);
      }
    }

    void drawBreadcrumbs() {
      int cursorX = -1;
      if (surfaces[0].pointerPort == 0) {
        cursorX = surfaces[0].getRelativeX();
      }
      String breadcrumbs = this->getBreadcrumbs();
      channels[0].ports[0].screen.clear();
      channels[0].ports[0].screen.drawBreadcrumbs(breadcrumbs, cursorX);
      surfaces[0].drawBackButton(0);
    }

    void drawContent() {
      OLED & contentScreen = channels[0].ports[1].screen;
      if (this->menuLevel < 3 && contentScreen.hasOptions) {
        contentScreen.clear();
        contentScreen.printBoxes();
        contentScreen.printLines();
        contentScreen.drawScrollbar();
      }
    }

    void tick() {
      Surface & previewSurface = surfaces[0];
      Surface & mainSurface = surfaces[1];
      this->menuLevel = this->getMenuLevel();

      if (previewSurface.facingUp) {
        OLED & previewScreen0 = channels[previewSurface.channel].ports[0].screen;
        OLED & previewScreen1 = channels[previewSurface.channel].ports[1].screen; 

        this->updatePointer();
        this->updateContent();
        
        this->drawBreadcrumbs();
        this->drawContent();
        
        previewSurface.drawPointer();

        if (gamepad.buttonApressed()) {
          if (previewSurface.pointerPort == 0) {
            if (previewScreen0.backButtonHovered) {
              if (this->menuLevel > 0) {
                this->levels[this->menuLevel] = "";
                this->populateOptions();
              }
            } else {
              if (previewScreen0.breadcrumbHovered > -1) {
                for (int i=0; i < 8; i++) {
                  if (i > previewScreen0.breadcrumbHovered) {
                    this->levels[i] = "";
                  }
                }
                this->populateOptions();
              }
            }
          }

          if (previewSurface.pointerPort == 1) {
            int index = previewScreen1.lineHovered;

            if (this->menuLevel == 0) {
              this->setProgram(index);
            }

            if (this->menuLevel == 1) {
              JsonArray & file = this->loadFromFile("/menu.json");
              previewSurface.populateScreen(1, file[1][activeProgram][1][index]);
              String programOption = file[1][activeProgram][1][index][0];
              this->levels[2] = programOption;
              programs[activeProgram]->setOption(index);
              programs[activeProgram]->becameActive();
            }

            if (this->menuLevel == 2) {
              String subOption = previewScreen1.lines[previewScreen1.lineHovered];
              this->levels[3] = subOption;
            }
          }
        }
      }

      if (mainSurface.facingUp) {
        if (activeProgram == -1) {
          OLED & screen = channels[mainSurface.channel].ports[0].screen;
          screen.clear();
          screen.printText("no program selected");
        }
      }
    }

    Interface() {}
};