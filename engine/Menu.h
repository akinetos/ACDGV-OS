class Menu:public Program {
  public:
    String segments[8];
    int level = 0;
    int address[8] = {0,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
    boolean pathChanged = false;
    
    String menuPath = "";
    String menuAddress = "";
    int optionsCount = 0;
    boolean hasOptions = false;
    String options[8];

    boolean show = true;

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
        this->populate(file[1]);
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

      this->menuAddress = this->getMenuAddress(); //menu.address = 
      this->menuPath = this->getMenuPath(); //menu.path =
    }

    void populateOptions() {
      OLED & screen = channels[0].ports[1].screen;
      screen.populate(this->getElement()[1]);
    }

    void closeProgram() {
      if (anyProgramActive()) {
        programs[activeProgram]->active = false;
      }
      activeProgram = -1;
      transitionType = 1;
      transition.init();
    }

    void close() {
      this->segments[this->level] = "";
      this->address[this->level-1] = NULL;
      this->level--;
      this->pathChanged = true;
      
      if (anyProgramActive()) {
        if (this->level < programs[activeProgram]->menuLevel) {
          this->closeProgram();
        }
      }
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
            if (this->level < programs[activeProgram]->menuLevel) {
              this->closeProgram();
            }
          }
        }

        if (this->pathChanged) {
          Surface * surface = & surfaces[0];
          this->menuAddress = this->getMenuAddress();
          this->menuPath = this->getMenuPath();
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

    void selectOption(int index) {
      this->level++;
      this->address[this->level-1] = index;
      JsonArray & element = this->getElement();
      String optionName = element[0];
      this->segments[this->level] = optionName;

      if (version == "8") {
        Surface * surface = & surfaces[0];
        this->populate(element[1]);

        int optionsCount = element[1].size();
        for (int i = 0; i < optionsCount; i++) {
          boolean hasCommand = element[1][i].size() == 3;
          if (hasCommand) {
            String commandType = element[1][i][2][0];
            if (commandType == "run") {
              String programName = element[1][i][2][1];
              int programIndex = -1;
              for (int i=0; i<programsCount; i++)
                if (programs[i]->name == programName)
                  programIndex = i;
              if (programIndex > -1)
                if (programs[programIndex]->menuLevel == -1)
                  programs[programIndex]->menuLevel = this->level + 1;
            }
          }
        }
      } else {
        OLED & screen = channels[0].ports[1].screen;
        screen.populate(element[1]);
      }

      JsonArray & command = element[2];
      if (command.size()) {
        this->execute(command);
      }
    }

    void reactToGamepadAction() {
      Surface * surface = & surfaces[0];
      JsonArray & element = this->getElement();

      if (surface->pointerPort == 0 && this->show && this->pathChanged) {
        if (version == "8") {
          this->populate(element[1]);
          JsonArray & command = element[2];
          if (command.size()) {
            this->execute(command);
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
          transitionType = 2;
          transition.init();
          this->selectOption(index);
        }
      }

      this->menuAddress = this->getMenuAddress();
      this->menuPath = this->getMenuPath();
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

      this->menuAddress = this->getMenuAddress();
      this->menuPath = this->getMenuPath();
    }

    boolean mainMenuHovered() {
      Surface * surface = & surfaces[0];
      boolean hovered = surface->pointerPort == 0 
        && (this->show || channels[0].ports[0].screen.closeButtonHovered);
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

    void draw() {
      Surface * surface = & surfaces[0];

      if (this->show) {
        this->drawPath(0);
        this->drawOptions();
      }

      if (this->level > 0) {
        OLED & screen = channels[0].ports[0].screen;
        screen.drawCloseButton();
      }
    }

    void execute(JsonArray & command) {
      String commandType = command[0];

      if (commandType == "run") {
        String programName = command[1];
        int programIndex = -1;
        for (int i=0; i<programsCount; i++)
          if (programs[i]->name == programName)
            programIndex = i;
        
        if (programIndex > -1) {
          boolean hasOption = command.size() == 3;
          boolean isActive = programs[programIndex]->active;

          if (hasOption) {
            int optionValue = command[2];
            programs[programIndex]->setOption(optionValue);
          }

          if (!isActive) {
            activeProgram = programIndex;
            if (!programs[activeProgram]->initialised)
              programs[activeProgram]->init();
            programs[activeProgram]->activate();
          }
        }
      }
    }

    void drawPath(int port) {
      Surface * surface = & surfaces[0];
      OLED & screen = channels[0].ports[port].screen;
      int cursorX = -1;
      int cursorY = -1;
      if (surface->pointerPort == 0) {
        cursorX = surface->getRelativeX();
        cursorY = surface->getRelativeY() - int(surface->getRelativeY() / screen.height);
      }
      screen.drawMenuAddress(menuAddress);
      screen.drawMenuPath(menuPath, cursorX, cursorY);
    }

    void drawOptions() {
      Surface * surface = & surfaces[0];
      if (version == "8") {
        for (int port = 1; port < this->optionsCount; port++) {
          OLED & screen = channels[0].ports[port].screen;
          int amount = 2;
          screen.hasOptions = amount > 0;
          screen.optionsCount = amount;
          screen.minOffsetY = -(screen.optionsCount * 10) + screen.height - 1;
          screen.lines[0] = String(port);
          screen.lines[1] = this->options[port];
          screen.printLines();
        }
      } else {
        OLED & screen = channels[0].ports[1].screen;
        if (screen.hasOptions) {
          screen.printBoxes();
          screen.printLines();
          screen.drawScrollbar();
        }
      }
    }

    void populate(JsonArray & list) {
      Surface * surface = & surfaces[0];
      if (list.size() > 0) {
        this->optionsCount = list.size();
        for (int i = 0; i < this->optionsCount; i++) {
          String optionName = list[i][0];
          this->options[i] = optionName;
        }
      } else {
        this->optionsCount = 0;
      }
      this->hasOptions = this->optionsCount > 0;

      if (version == "8") {
        for (int port = 1; port < this->optionsCount; port++) {
          OLED & screen = channels[0].ports[port].screen;
          screen.lineSelected = -1;
          for (int i=0; i<=20; i++) {
            screen.lineScrollWidth[i] = 0;
          }
          screen.offsetY = 0;
        }
      }

      surface->refreshScreens();
    }

    Menu() {}
};