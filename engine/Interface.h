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
      Surface * surface = & surfaces[0];
      JsonArray & file = loadFromFile("/config/menu.json");
      String optionName = file[0];

      if (version == "8") {
        surface->populateInit(file[1]);
      } else {
        channels[0].ports[1].screen.populate(file[1]);
      }

      for (int i=this->pathLevel+1; i<8; i++) {
        this->segments[i] = "";
      }
      this->segments[0] = optionName;
      
      for (int i=0; i<programsCount; i++) {
        if (programs[i]->active) {
          programs[i]->init();
        }
      }

      surface->menuPath = this->getPath();
    }

    void populateOptions() {
      OLED & screen = channels[0].ports[1].screen;
      JsonArray & file = loadFromFile("/config/menu.json");
      if (this->pathLevel == 3) {
        screen.populate(file[1][address[0]][1][address[1]][1][address[2]][1]);
      }
      if (this->pathLevel == 2) {
        screen.populate(file[1][address[0]][1][address[1]][1]);
      }
      if (this->pathLevel == 1) {
        screen.populate(file[1][address[0]][1]);
      }
      if (this->pathLevel == 0) {
        screen.populate(file[1]);
      }
    }

    void populateOptions2() {
      Surface * surface = & surfaces[0];
      JsonArray & file = loadFromFile("/config/menu.json");
      if (this->pathLevel == 3) {
        surface->populateInit(file[1][address[0]][1][address[1]][1][address[2]][1]);
      }
      if (this->pathLevel == 2) {
        surface->populateInit(file[1][address[0]][1][address[1]][1]);
      }
      if (this->pathLevel == 1) {
        surface->populateInit(file[1][address[0]][1]);
      }
      if (this->pathLevel == 0) {
        surface->populateInit(file[1]);
      }
    }

    void deactivatePrograms() {
      for (int i=0; i<programsCount; i++) {
        programs[i]->active = false;
      }
    }

    //updates this->segments and this->address when back or segment selected
    void updateMenu() {
      OLED & screen = channels[0].ports[0].screen;

      if (screen.textScroll != 0) {
        screen.needsRefresh = true;
      }

      if (gamepad.buttonApressed() && this->mainMenuHovered()) {
        this->pathChanged = false;

        if (screen.backButtonHovered) {
          if (this->pathLevel > 0) {
            this->segments[this->pathLevel] = "";
            this->address[this->pathLevel-1] = NULL;
            this->pathLevel--;
            this->pathChanged = true;
            this->deactivatePrograms(); //TODO should not be here
          }
        } else {
          if (screen.pathSegmentHovered > -1) {
            this->pathLevel = screen.pathSegmentHovered;
            for (int i=this->pathLevel+1; i < 8; i++) {
              this->segments[i] = "";
              this->address[i-1] = NULL;
              this->pathChanged = true;
            }
          }
        }

        if (this->pathChanged) {
          Surface * surface = & surfaces[0];
          surface->refreshScreens();
          surface->menuPath = this->getPath(); //TODO investigate this
        }
      }
    }

    JsonArray & getElement() {
      JsonArray & file = loadFromFile("/config/menu.json");
      if (this->pathLevel == 0) {
        return file;
      }
      if (this->pathLevel == 1) {
        return file[1][address[0]];
      }
      if (this->pathLevel == 2) {
        return file[1][address[0]][1][address[1]];
      }
      if (this->pathLevel == 3) {
        return file[1][address[0]][1][address[1]][1][address[2]];
      }
    }

    void selectOption(int index) {
      this->pathLevel++;
      this->address[this->pathLevel-1] = index;

      OLED & screen = channels[0].ports[1].screen;
      JsonArray & element = this->getElement();

      String optionName = element[0];
      this->segments[this->pathLevel] = optionName;

      screen.populate(element[1]);

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
          
          programs[programIndex]->active = !programs[programIndex]->active;
          if (programs[programIndex]->active) {
            if (!programs[programIndex]->initialised) {
              programs[programIndex]->init();
            }
            if (element[2].size() == 3) {
              int programOption = element[2][2];
              programs[programIndex]->setOption(programOption);
            }
            programs[programIndex]->justActivated();
          }
        }
      }
    }

    void selectOption2(int optionIndex) {
      this->pathLevel++;
      this->address[this->pathLevel-1] = optionIndex;

      Surface * surface = & surfaces[0];
      JsonArray & element = this->getElement();
      
      String optionName = element[0];
      this->segments[this->pathLevel] = optionName;
      
      surface->populateInit(element[1]);
      surface->refreshScreens();

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
          
          this->deactivatePrograms();
          programs[programIndex]->active = true;
          if (programs[programIndex]->active) {
            if (!programs[programIndex]->initialised) {
              programs[programIndex]->init();
            }
            if (element[2].size() == 3) {
              int programOption = element[2][2];
              programs[programIndex]->setOption(programOption);
            }
            programs[programIndex]->justActivated();
          }
        }
      }
    }

    void updatePrograms() {
      this->anyProgramActive = false;
      for (int i=0; i<programsCount; i++) {
        if (programs[i]->active) {
          this->anyProgramActive = true;
          programs[i]->update();
        }
      }
    }

    void reactToGamepadAction() {
      Surface * surface = & surfaces[0];

      if (surface->pointerPort == 0 && this->showMenu && this->pathChanged) {
        if (version == "8") {
          
          //TODO temporal solution
          for (int i=1; i<8; i++) {
            OLED & screen = channels[surface->channel].ports[i].screen;
            screen.clear();
          }
          //-----------

          this->populateOptions2();
        } else {
          this->populateOptions();
        }
      }

      if (surface->pointerPort > 0) {
        if (version == "8") {
          int index = surface->pointerPort;

          //TODO temporal solution
          for (int i=1; i<8; i++) {
            OLED & screen = channels[surface->channel].ports[i].screen;
            screen.clear();
          }
          //-----------

          this->selectOption2(index);
        } else {
          OLED & screen = channels[0].ports[1].screen;
          int index = screen.lineHovered;
          this->selectOption(index);
        }
      }

      surface->menuPath = this->getPath();
    }

    void reactToKeypadAction() {
      char button = keypad.device.getButton();
      if (button != '*' && button != '#') {
        int index = button - 48;
        if (this->anyProgramActive) {
          programs[index]->active = !programs[index]->active;
        } else {
          this->selectOption2(index);
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
          this->pathLevel--;
          this->populateOptions2();
        }
      }
    }

    boolean mainMenuHovered() {
      Surface * surface = & surfaces[0];
      boolean hovered = surface->pointerPort == 0 
        && (this->showMenu || channels[0].ports[0].screen.backButtonHovered);
      return hovered;
    }

    void drawActivePrograms() {
      for (int i=0; i<programsCount; i++) {
        if (programs[i]->active) {
          programs[i]->tick();
        }
      }
    }

    void tick() {
      Surface * surface = & surfaces[0];

      if (surface->facingUp) {
        this->updatePointer();
        this->updateOptions();
        this->updatePrograms();
        this->updateMenu();

        surface->clear();

        if (this->anyProgramActive) {
          this->drawActivePrograms();
        }

        if (this->showMenu) {
          surface->drawMenuPath(0);
          surface->drawMenuOptions();
        }

        if (this->pathLevel > 0) {
          surface->drawBackButton(0);
        }
        
        surface->drawPointer();
      }
    }

    Interface() {}
};