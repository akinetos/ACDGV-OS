class Interface:public Program {
  public:
    String segments[8];
    int pathLevel = 0;
    int address[8] = {0,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
    boolean showMenu = true;
    boolean anyProgramActive = false;
    boolean pathChanged = false;

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

    void refreshScreens() {
      for (int i=0; i<8; i++) {
        channels[0].ports[i].screen.needsRefresh = true;
      }
    }

    void updateOptions() {
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
          if (gamepad.connected) {
            previewScreen1.updateScrollbar(gamepad.axisY);
          }
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
      if (!this->anyProgramActive) {
        Surface & surface = surfaces[0];
        if (surface.pointerPositionX > 120) {
          surface.pointerPositionY = surface.pointerPreviousPositionY;
          if (surface.pointerPortChanged()) {
            surface.pointerPort = surface.pointerPreviousPort;
          }
        }
      }
    }

    void init() {
      JsonArray & file = loadFromFile("/config/menu.json");
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

    void updatePathLevel() {
      this->pathLevel = 0;
      for (int i=0; i<8; i++) {
        if (this->segments[i] != "") {
          this->pathLevel = i;
        }
      }
    }

    void populateOptions() {
      OLED & screen = channels[0].ports[1].screen;
      JsonArray & file = loadFromFile("/config/menu.json");
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

    void deactivatePrograms() {
      for (int i=0; i<programsCount; i++) {
        programs[i]->active = false;
      }
    }

    void updateMenu() {
      OLED & screen = channels[0].ports[0].screen;
      this->pathChanged = false;
      if (screen.backButtonHovered) {
        if (this->pathLevel > 0) {
          this->segments[this->pathLevel] = "";
          this->address[this->pathLevel] = NULL;
          this->pathChanged = true;
        }
      } else {
        if (screen.pathSegmentHovered > -1) {
          for (int i=0; i < 8; i++) {
            if (i > screen.pathSegmentHovered) {
              this->segments[i] = "";
              this->address[i] = NULL;
              this->pathChanged = true;
            }
          }
        }
      }
      if (this->pathChanged) {
        this->updatePathLevel();
        this->deactivatePrograms();
        this->refreshScreens();
        this->showMenu = true;
      }
    }

    JsonArray & getElement() {
      JsonArray & file = loadFromFile("/config/menu.json");
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

    void selectOption(int index) {
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
          int programIndex = -1;
          if (programName == "batterfly") programIndex = 0;
          if (programName == "gravity") programIndex = 1;
          if (programName == "vv") programIndex = 2;
          if (programName == "logo") programIndex = 3;
          if (programName == "telephone") programIndex = 4;
          if (programName == "i2c") programIndex = 5;
          programs[programIndex]->counter = 0;
          programs[programIndex]->active = true;
          if (!programs[programIndex]->initialised) {
            programs[programIndex]->init();
          }
          if (element[2].size() == 3) {
            int programOption = element[2][2];
            programs[programIndex]->setOption(programOption);
          }
          programs[programIndex]->justActivated();
          this->showMenu = false;
        }
      } else {
        this->deactivatePrograms();
      }
    }

    void updatePrograms() {
      this->anyProgramActive = false;
      for (int i=0; i<programsCount; i++) {
        if (programs[i]->active) {
          this->anyProgramActive = true;
        }
      }
    }

    void tick() {
      Surface * surface = & surfaces[0];

      if (surface->facingUp) {
        this->updatePathLevel();
        this->updatePrograms();
        this->updatePointer();
        this->updateOptions();
        
        if (gamepad.buttonApressed()) {
          if (
            surface->pointerPort == 0 
            && (this->showMenu || channels[0].ports[0].screen.backButtonHovered)
          ) {
            this->updateMenu();
          }
        }

        surface->clear();

        if (this->anyProgramActive) {
          for (int i=0; i<programsCount; i++) {
            if (programs[i]->active) {
              programs[i]->tick();
            }
          }
        }

        if (this->showMenu) {
          String path = this->getPath();
          surface->drawMenu(path);
          surface->drawOptions(1);
        }

        if (this->pathLevel > 0) {
          surface->drawBackButton(0);
        }
        
        surface->drawPointer();

        if (gamepad.buttonApressed()) {
          if (surface->pointerPort == 0 && this->showMenu) {
            if (this->pathChanged) {
              this->populateOptions();
            }
          }
          if (surface->pointerPort == 1 && !this->anyProgramActive) {
            OLED & screen = channels[0].ports[1].screen;
            int index = screen.lineHovered;
            this->selectOption(index);
          }
        }

        if (keypad.anyKeyPressed()) {
          char button = keypad.device.getButton();
          if (button != '*' && button != '#') {
            int index = button - 48;
            if (this->anyProgramActive) {
              programs[index]->active = !programs[index]->active;
            } else {
              this->selectOption(index);
            }
          }
          if (button == '#') {
            if (this->anyProgramActive) {
              this->showMenu = !this->showMenu;
            }
          }
          if (button == '*') {
            if (this->anyProgramActive) {
              this->deactivatePrograms();
            }
            if (this->pathLevel > 0) {
              this->segments[this->pathLevel] = "";
              this->address[this->pathLevel] = NULL;
              this->updatePathLevel();
              this->populateOptions();
            }
          }
        }
      }
    }

    Interface() {}
};