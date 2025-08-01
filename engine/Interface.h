class Interface:public Program {
  public:
    String segments[8];
    int pathLevel = 0;
    int activeProgram = -1;
    String programOption = "";
    String programSubOption = "";
    int frameNumber = 0;
    int becameActiveTime = 0;

    String getPath() {
      String output = "";
      for (int i=0; i<8; i++) {
        if (this->segments[i] != "") {
          if (output != "") {
            output += "/";
          }
          output += segments[i];
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
        this->segments[i] = "";
      }
      this->segments[0] = root;
      if (this->activeProgram > -1) {
        this->setProgram(this->activeProgram);
      }
    }

    int getPathLevel() {
      int output = 0;
      for (int i=0; i<8; i++) {
        if (this->segments[i] != "") {
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
      this->activeProgram = index;
      JsonArray & file = this->loadFromFile("/menu.json");
      surfaces[0].populateScreen(1, file[1][this->activeProgram]);
      surfaces[1].clear();
      String programName = file[1][this->activeProgram][0];
      this->segments[1] = programName;
    }

    void populateOptions() {
      Surface & previewSurface = surfaces[0];
      int newpathLevel = this->getPathLevel();
      JsonArray & file = this->loadFromFile("/menu.json");
      if (newpathLevel == 1) {
        previewSurface.populateScreen(1, file[1][this->activeProgram]);
      }
      if (newpathLevel == 0) {
        previewSurface.populateScreen(1, file);
      }
    }

    void drawPath() {
      int cursorX = -1;
      if (surfaces[0].pointerPort == 0) {
        cursorX = surfaces[0].getRelativeX();
      }
      String path = this->getPath();
      if (this->pathLevel < 2) {
        channels[0].ports[0].screen.clear();
      }
      channels[0].ports[0].screen.drawPath(path, cursorX);
    }

    void drawContent() {
      OLED & contentScreen = channels[0].ports[1].screen;
      if (this->pathLevel < 3 && contentScreen.hasOptions) {
        contentScreen.clear();
        contentScreen.printBoxes();
        contentScreen.printLines();
        contentScreen.drawScrollbar();
      }
    }

    void reactToPathPressed() {
      OLED & screen = channels[0].ports[0].screen;
      if (screen.backButtonHovered) {
        if (this->pathLevel > 0) {
          this->segments[this->pathLevel] = "";
          this->populateOptions();
        }
      } else {
        if (screen.pathSegmentHovered > -1) {
          for (int i=0; i < 8; i++) {
            if (i > screen.pathSegmentHovered) {
              this->segments[i] = "";
            }
          }
          this->populateOptions();
        }
      }
    }

    void reactToContentPressed() {
      OLED & screen = channels[0].ports[1].screen;
      int index = screen.lineHovered;

      if (this->pathLevel == 0) {
        this->setProgram(index);
      }

      if (this->pathLevel == 1) {
        JsonArray & file = this->loadFromFile("/menu.json");
        surfaces[0].populateScreen(1, file[1][this->activeProgram][1][index]);
        String programOption = file[1][this->activeProgram][1][index][0];
        this->programOption = programOption;
        this->segments[2] = this->programOption;
        programs[this->activeProgram]->setOption(index);
        programs[this->activeProgram]->becameActive();
        this->becameActiveTime = millis();
      }

      if (this->pathLevel == 2) {
        this->programSubOption = screen.lines[screen.lineHovered];
        this->segments[3] = this->programSubOption;
      }
    }

    void updatePath() {
      this->pathLevel = this->getPathLevel();
    }

    void tick() {
      this->frameNumber++;

      if (this->activeProgram > -1 && this->activeProgram < programsCount) {
        programs[this->activeProgram]->tick();
      }
      
      this->updatePath();

      if (surfaces[0].facingUp) {
        this->updatePointer();
        this->updateContent();
        
        if (this->pathLevel < 2) {
          this->drawPath();
        } else {
          if (millis() - this->becameActiveTime < 3000) {
            this->drawPath();
          } else {
            if (millis() - this->becameActiveTime < 4500) {
              if (this->frameNumber % 2 == 0) {
                this->drawPath();
              }
            }
          }
        }

        surfaces[0].drawBackButton(0);
        
        this->drawContent();
        
        surfaces[0].drawPointer();

        if (gamepad.buttonApressed()) {
          if (surfaces[0].pointerPort == 0) {
            this->reactToPathPressed();
          }

          if (surfaces[0].pointerPort == 1) {
            this->reactToContentPressed();
          }
        }
      }

      if (surfaces[1].facingUp) {
        if (this->activeProgram == -1) {
          OLED & screen = channels[surfaces[1].channel].ports[0].screen;
          screen.clear();
          screen.printText("no program selected");
        }
      }
    }

    Interface() {}
};