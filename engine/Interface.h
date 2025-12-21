class Interface:public Program {
  public:
    String segments[8];
    int pathLevel = 0;
    int address[8] = {0,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
    boolean showMenu = true;
    boolean anyProgramActive = false;
    boolean pathChanged = false;

    String getMenuPath() {
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

    String getMenuAddress() {
      String output = "";
      for (int i=0; i<8; i++) {
        if (this->address[i] != NULL) {
          if (output != "") {
            output += "/";
          }
          output += this->address[i];
        }
      }
      return output;
    }

    void updateOptions() {
      Surface & surface = surfaces[0];
      int x = surface.getRelativeX();
      int y = surface.getRelativeY();

      if (surface.pointerPortChanged()) {
        OLED & previousScreen = channels[surface.channel].ports[surface.pointerPreviousPort].screen;
        previousScreen.resetLineHovered();
      }

      OLED & screen = channels[surface.channel].ports[surface.pointerPort].screen;
      screen.lineHovered = (int)((y - screen.offsetY) / 10);

      if (surface.pointerPort == 0) {
        screen.closeButtonHovered = x > 118 && y < 16;
      }
      
      /*
      OLED & previewScreen0 = channels[surface.channel].ports[0].screen;
      OLED & previewScreen1 = channels[surface.channel].ports[1].screen;
      if (surface.pointerPort == 0) {
        previewScreen0.closeButtonHovered = x > 118 && y < 16;
      }

      if (surface.pointerPort == 1) {
        previewScreen1.scrollbarHovered = x > (previewScreen1.width-10);
        if (previewScreen1.scrollbarHovered) {
          if (devices[4]->connected) {
            previewScreen1.updateScrollbar(devices[4]->axisY);
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
      */
    }

    void updatePointer() {
      if (!this->anyProgramActive) {
        Surface & surface = surfaces[0];
        if (surface.pointerPositionX > 120 && channels[surface.channel].ports[surface.pointerPort].screen.scrollbarHovered) {
          surface.pointerPositionY = surface.pointerPreviousPositionY;
          if (surface.pointerPortChanged()) {
            surface.pointerPort = surface.pointerPreviousPort;
          }
        }
      }
    }

    void init() {
      Surface * surface = & surfaces[0];
      JsonArray & file = storage.load("/config/menu.json");
      String optionName = file[0];

      if (version == "8") {
        surface->populate(file[1]);
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

      surface->menuAddress = this->getMenuAddress();
      surface->menuPath = this->getMenuPath();
    }

    void populateOptions() {
      OLED & screen = channels[0].ports[1].screen;
      JsonArray & file = storage.load("/config/menu.json");
      if (this->pathLevel == 4) {
        screen.populate(file[1][address[0]][1][address[1]][1][address[2]][1][address[3]][1]);
      }
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

    void deactivatePrograms() {
      for (int i=0; i<programsCount; i++) {
        programs[i]->active = false;
      }
      channels[0].ports[1].screen.textScroll = 0;
    }

    void updateMenu() {
      OLED & screen = channels[0].ports[0].screen;

      if (screen.textScroll != 0) {
        screen.needsRefresh = true;
      }

      if (devices[4]->shortPress && this->mainMenuHovered()) {
        this->pathChanged = false;

        if (screen.closeButtonHovered) {
          if (this->pathLevel > 0) {
            this->segments[this->pathLevel] = "";
            this->address[this->pathLevel-1] = NULL;
            this->pathLevel--;
            this->pathChanged = true;
            this->deactivatePrograms();
          }
        } else {
          if (screen.pathSegmentHovered > -1) {
            this->pathLevel = screen.pathSegmentHovered;
            for (int i=this->pathLevel+1; i < 8; i++) {
              this->segments[i] = "";
              this->address[i-1] = NULL;
              this->pathChanged = true;
            }
            this->deactivatePrograms();
          }
        }

        if (this->pathChanged) {
          Surface * surface = & surfaces[0];
          surface->menuAddress = this->getMenuAddress();
          surface->menuPath = this->getMenuPath();
        }
      }
    }

    JsonArray & getElement() {
      JsonArray & file = storage.load("/config/menu.json");
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
      if (this->pathLevel == 4) {
        return file[1][address[0]][1][address[1]][1][address[2]][1][address[3]];
      }
    }

    void run(JsonArray & program) {
      String programName = program[2][1];
      int programIndex = -1;
      if (programName == "batterfly") programIndex = 0;
      if (programName == "gravity") programIndex = 1;
      if (programName == "vv") programIndex = 2;
      if (programName == "logo") programIndex = 3;
      if (programName == "telephone") programIndex = 4;
      if (programName == "i2c") programIndex = 5;
      if (programName == "contacts") programIndex = 6;
      if (programName == "NFC") programIndex = 7;
      if (programName == "battery") programIndex = 8;
      
      this->deactivatePrograms();
      programs[programIndex]->active = true;
      if (!programs[programIndex]->initialised) {
        programs[programIndex]->init();
      }
      if (program[2].size() == 3) {
        int programOption = program[2][2];
        programs[programIndex]->setOption(programOption);
      }
      programs[programIndex]->justActivated();
    }

    void selectOption(int index) {
      this->pathLevel++;
      this->address[this->pathLevel-1] = index;
      JsonArray & element = this->getElement();
      String optionName = element[0];
      this->segments[this->pathLevel] = optionName;

      if (version == "8") {
        Surface * surface = & surfaces[0];
        surface->populate(element[1]);
      } else {
        OLED & screen = channels[0].ports[1].screen;
        screen.populate(element[1]);
      }

      if (element[2]) {
        if (element[2][0] == "run") {
          this->run(element);
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
          JsonArray & element = this->getElement();
          surface->populate(element[1]);
          if (element[2]) {
            if (element[2][0] == "run") {
              this->run(element);
            }
          }
        } else {
          this->populateOptions();
        }
      }

      if (surface->pointerPort > 0) {
        int index;
        if (version == "8") {
          index = surface->pointerPort;
        } else {
          OLED & screen = channels[0].ports[1].screen;
          index = screen.lineHovered;
        }
        this->selectOption(index);
      }

      surface->menuAddress = this->getMenuAddress();
      surface->menuPath = this->getMenuPath();
    }

    void reactToKeypadAction() {
      /*
      Surface * surface = & surfaces[0];
      char button = keypad.device.getButton();
      if (button != '*' && button != '#' && !this->anyProgramActive) {
        int index = button - 48;
        this->selectOption(index);
      }
      */
    }

    boolean mainMenuHovered() {
      Surface * surface = & surfaces[0];
      boolean hovered = surface->pointerPort == 0 
        && (this->showMenu || channels[0].ports[0].screen.closeButtonHovered);
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

        if (devices[4]->shortPress) {
          this->reactToGamepadAction();
        }

        //if (keypad.anyKeyPressed()) {
        //  this->reactToKeypadAction();
        //}

        surface->clear();

        if (this->anyProgramActive) {
          this->drawActivePrograms();
        }

        if (this->showMenu) {
          surface->drawMenuPath(0);
          surface->drawMenuOptions();
        }

        if (this->pathLevel > 0) {
          surface->drawCloseButton(0);
        }
        
        surface->drawPointer();

        if (surface->pointerPortChanged()) {
          surface->pointerPreviousPort = surface->pointerPort;
        }
      }
    }

    Interface() {}
};