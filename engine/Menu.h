class Menu:public Program {
  public:
    String segments[8];
    int level = 0;
    int address[8] = {0,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
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
      Surface * surface = & surfaces[0];
      int x = surface->getRelativeX();
      int y = surface->getRelativeY();

      if (surface->pointerPortChanged()) {
        OLED & previousScreen = channels[surface->channel].ports[surface->pointerPreviousPort].screen;
        previousScreen.resetLineHovered();
      }

      OLED & screen = channels[surface->channel].ports[surface->pointerPort].screen;
      screen.lineHovered = (int)((y - screen.offsetY) / 10);

      if (surface->pointerPort == 0) {
        screen.closeButtonHovered = x > 118 && y < 16;
      }
    }

    void updatePointer() {
      Surface * surface = & surfaces[0];
      if (surface->pointerPositionX > 120 && channels[surface->channel].ports[surface->pointerPort].screen.scrollbarHovered) {
        surface->pointerPositionY = surface->pointerPreviousPositionY;
        if (surface->pointerPortChanged()) {
          surface->pointerPort = surface->pointerPreviousPort;
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

      for (int i=this->level+1; i<8; i++) {
        this->segments[i] = "";
      }
      this->segments[0] = optionName;
      
      for (int i=0; i<programsCount; i++) {
        if (programs[i]->active) {
          programs[i]->init();
        }
      }

      //do przerobienia
      surface->menuAddress = this->getMenuAddress(); //menu.address = 
      surface->menuPath = this->getMenuPath(); //menu.path =
    }

    void populateOptions() {
      OLED & screen = channels[0].ports[1].screen;
      screen.populate(this->getElement()[1]);
    }

    void deactivatePrograms() {
      for (int i=0; i<programsCount; i++) {
        programs[i]->active = false;
      }
    }

    void close() {
      this->segments[this->level] = "";
      this->address[this->level-1] = NULL;
      this->level--;
      this->pathChanged = true;
      this->deactivatePrograms();
    }
    
    void updateMenu() {
      OLED & screen = channels[0].ports[0].screen;

      if (screen.textScroll != 0) {
        screen.needsRefresh = true;
      }

      if (devices[4]->shortPress && this->mainMenuHovered()) {
        this->pathChanged = false;

        if (screen.closeButtonHovered) {
          if (this->level > 0) {
            this->close();
          }
        } else {
          if (screen.pathSegmentHovered > -1) {
            this->level = screen.pathSegmentHovered;
            for (int i=this->level+1; i < 8; i++) {
              this->segments[i] = "";
              this->address[i-1] = NULL;
              this->pathChanged = true;
            }
            //this->deactivatePrograms();
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
      JsonArray * node = &file;
      for (int i=0; i<this->level; i++) {
        JsonArray & children = (*node)[1];
        JsonArray & next = children[address[i]];
        node = &next;
      }
      return *node;
    }

    void run(JsonArray & command) {
      String commandType = command[0];
      String programName = command[1];
      int optionValue = command[2];
      boolean hasOption = command.size() == 3;
      int programIndex = -1;

      for (int i=0; i<programsCount; i++) {
        if (programs[i]->name == programName) {
          programIndex = i;
        }
      }
      
      if (programIndex > -1) {
        if (hasOption) {
          programs[programIndex]->setOption(optionValue);
        } else {
          programs[programIndex]->active = !programs[programIndex]->active;
          if (programs[programIndex]->active) {
            if (!programs[programIndex]->initialised) {
              programs[programIndex]->init();
            }
            programs[programIndex]->justActivated();
          }
        }
      }
    }

    void selectOption(int index) {
      this->level++;
      this->address[this->level-1] = index;
      JsonArray & element = this->getElement();
      String optionName = element[0];
      this->segments[this->level] = optionName;

      if (version == "8") {
        Surface * surface = & surfaces[0];
        surface->populate(element[1]);
      } else {
        OLED & screen = channels[0].ports[1].screen;
        screen.populate(element[1]);
      }

      JsonArray & command = element[2];
      if (command.size()) {
        this->run(command);
      }
    }

    void reactToGamepadAction() {
      Surface * surface = & surfaces[0];
      JsonArray & element = this->getElement();

      if (surface->pointerPort == 0 && surface->showMenu && this->pathChanged) {
        if (version == "8") {
          surface->populate(element[1]);
          JsonArray & command = element[2];
          if (command.size()) {
            this->run(command);
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
        
        if (element[1].size() > 0 && index < element[1].size()) {
          this->selectOption(index);
        }
      }

      surface->menuAddress = this->getMenuAddress();
      surface->menuPath = this->getMenuPath();
    }

    void reactToKeypadAction() {
      Surface * surface = & surfaces[0];
      JsonArray & element = this->getElement();
      int index;
      char button = devices[5]->buttonPressed;

      if (button != '*' && button != '#') {
        int index = button - 48;
        if (element[1].size() > 0 && index < element[1].size()) {
          this->selectOption(index);
        }
      }

      if (button == '*') {
        if (this->level > 0) {
          this->close();
        }
      }

      surface->menuAddress = this->getMenuAddress();
      surface->menuPath = this->getMenuPath();
    }

    boolean mainMenuHovered() {
      Surface * surface = & surfaces[0];
      boolean hovered = surface->pointerPort == 0 
        && (surface->showMenu || channels[0].ports[0].screen.closeButtonHovered);
      return hovered;
    }

    void tick() {
      Surface * surface = & surfaces[0];

      if (surface->facingUp) {
        this->updatePointer();
        this->updateOptions();
        this->updateMenu();

        if (devices[4]->shortPress) {
          this->reactToGamepadAction();
        }

        if (devices[5]->shortPress) {
          this->reactToKeypadAction();
        }
      }
    }

    Menu() {}
};