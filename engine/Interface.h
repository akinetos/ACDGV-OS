class Interface:public Program {
  public:
    String segments[8];
    int pathLevel = 0;
    int frameNumber = 0;
    int address[8] = {0,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
    boolean showPath = true;

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
      JsonArray & file = this->loadFromFile("/menu.json");
      String root = file[0];
      channels[0].ports[1].screen.populate(file);
      for (int i=0; i<8; i++) {
        this->segments[i] = "";
      }
      this->segments[0] = root;
      for (int i=0; i<programsCount; i++) {
        if (programs[i]->active) {
          programs[i]->init();
        }
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

    void populateOptions() {
      OLED & screen = channels[0].ports[1].screen;
      JsonArray & file = this->loadFromFile("/menu.json");
      if (this->pathLevel == 3) {
        screen.populate(file[1][address[1]][1][address[2]][1][address[3]]);
      }
      if (this->pathLevel == 2) {
        screen.populate(file[1][address[1]][1][address[2]]);
      }
      if (this->pathLevel == 1) {
        screen.populate(file[1][address[1]]);
      }
      if (this->pathLevel == 0) {
        screen.populate(file);
      }
    }

    void drawPath() {
      int cursorX = -1;
      if (surfaces[0].pointerPort == 0) {
        cursorX = surfaces[0].getRelativeX();
      }
      String path = this->getPath();
      if (this->pathLevel < 3) {
        channels[0].ports[0].screen.clear();
      }
      channels[0].ports[0].screen.drawPath(path, cursorX);
      surfaces[0].drawBackButton(0);
    }

    void drawContent() {
      OLED & contentScreen = channels[0].ports[1].screen;
      if (this->pathLevel < 4 && contentScreen.hasOptions) {
        contentScreen.clear();
        contentScreen.printBoxes();
        contentScreen.printLines();
        contentScreen.drawScrollbar();
      }
    }

    void reactToPathPressed() {
      OLED & screen = channels[0].ports[0].screen;
      boolean pathChanged = false;
      if (screen.backButtonHovered) {
        if (this->pathLevel > 0) {
          this->segments[this->pathLevel] = "";
          this->address[this->pathLevel] = NULL;
          pathChanged = true;
        }
      } else {
        if (screen.pathSegmentHovered > -1) {
          for (int i=0; i < 8; i++) {
            if (i > screen.pathSegmentHovered) {
              this->segments[i] = "";
              this->address[i] = NULL;
              pathChanged = true;
            }
          }
        }
      }
      if (pathChanged) {
        this->updatePath();
        this->populateOptions();
      }
    }

    JsonArray & getElement() {
      JsonArray & file = this->loadFromFile("/menu.json");
      if (this->pathLevel == 0) {
        return file[1][address[1]];
      }
      if (this->pathLevel == 1) {
        return file[1][address[1]][1][address[2]];
      }
      if (this->pathLevel == 2) {
        return file[1][address[1]][1][address[2]][1][address[3]];
      }
      if (this->pathLevel == 3) {
        return file[1][address[1]][1][address[2]][1][address[3]][1][address[4]];
      }
    }

    void reactToContentPressed(int index) {
      OLED & screen = channels[0].ports[1].screen;
      this->address[this->pathLevel + 1] = index;
      for (int i=this->pathLevel+2; i<8; i++) {
        this->address[i] = NULL;
      }
      JsonArray & element = this->getElement();
      String optionName = element[0];
      screen.populate(element);
      this->segments[this->pathLevel + 1] = optionName;

      if (element[2]) {
        if (element[2][0] == "run") {
          String programName = element[2][1];
          int programOption = element[2][2];
          int programIndex = -1;
          if (programName == "batterfly") programIndex = 0;
          if (programName == "gravity") programIndex = 1;
          if (programName == "vv") programIndex = 2;
          if (programName == "logo") programIndex = 3;
          if (programName == "telephone") programIndex = 4;
          programs[programIndex]->active = !programs[programIndex]->active;
          if (programs[programIndex]->becameActiveTime == 0) {
            programs[programIndex]->init();
          }
          programs[programIndex]->setOption(programOption);
          programs[programIndex]->becameActive();
        }
      } else {
        for (int i=0; i<programsCount; i++) {
          programs[i]->active = false;
        }
      }
    }

    void updatePath() {
      this->pathLevel = this->getPathLevel();
    }

    void tick() {
      boolean anyProgramActive = false;
      for (int i=0; i<programsCount; i++) {
        if (programs[i]->active) {
          anyProgramActive = true;
        }
      }

      if (surfaces[0].facingUp) {
        surfaces[0].clear();
      }

      this->updatePath();

      for (int i=0; i<programsCount; i++) {
        if (programs[i]->active) {
          programs[i]->tick();
        }
      }

      if (surfaces[0].facingUp) {
        this->updatePointer();
        this->updateContent();
        if (this->showPath) {
          this->drawPath();
        }
        this->drawContent();
        surfaces[0].drawPointer();
        
        if (gamepad.buttonApressed()) {
          if (surfaces[0].pointerPort == 0) {
            this->reactToPathPressed();
          }
          if (surfaces[0].pointerPort == 1) {
            OLED & screen = channels[0].ports[1].screen;
            int index = screen.lineHovered;
            this->reactToContentPressed(index);
          }
        }

        if (keypad.anyKeyPressed()) {
          char button = keypad.device.getButton();
          if (button != '*' && button != '#') {
            int index = button - 48;
            if (anyProgramActive) {
              programs[index]->active = !programs[index]->active;
            } else {
              this->reactToContentPressed(index);
            }
          }
          if (button == '#') {
            if (anyProgramActive) {
              this->showPath = !this->showPath;
            }
          }
          if (button == '*') {
            if (this->pathLevel > 0) {
              this->segments[this->pathLevel] = "";
              this->address[this->pathLevel] = NULL;
              this->updatePath();
              this->populateOptions();
            }
          }
        }
      }

      if (surfaces[1].facingUp) {
        if (!anyProgramActive) {
          OLED & screen = channels[surfaces[1].channel].ports[0].screen;
          screen.clear();
          screen.printText("no program selected");
        }
      }

      this->frameNumber++;
    }

    Interface() {}
};