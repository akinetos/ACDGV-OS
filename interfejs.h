class interfejs {
  public:
    int address[8];
    String segments[8];
    String options[8];

    String segmentsPath = "";
    String addressPath = "";
    int optionsCount = 0;

    boolean show = true;
    boolean changed = false;
    int aktywnaWarstwa = 0;

    String version = "8";

    int deviceIndexGamepad = -1;
    int deviceIndexKeypad = -1;

    String getSegmentsPath() {
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

    String getAddressPath() {
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
      przestrzen & surface = przestrzenie[0];
      int x = surface.getRelativeX();
      int y = surface.getRelativeY();

      if (surface.pointerPortChanged()) {
        ekran & previousScreen = i2c.kanaly[surface.channel].porty[surface.pointerPreviousPort].screen;
        previousScreen.resetLineHovered();
      }

      ekran & screen = i2c.kanaly[surface.channel].porty[surface.pointerPort].screen;
      screen.lineHovered = (int)((y - screen.offsetY) / 10);

      if (surface.pointerPort == 0) {
        screen.closeButtonHovered = x > 118 && y < 16;
      }
    }

    void updatePointer() {
      przestrzen & surface = przestrzenie[0];
      if (surface.pointerPositionX > 120 && i2c.kanaly[surface.channel].porty[surface.pointerPort].screen.scrollbarHovered) {
        surface.pointerPositionY = surface.pointerPreviousPositionY;
        if (surface.pointerPortChanged()) {
          surface.pointerPort = surface.pointerPreviousPort;
        }
      }
    }

    void inicjuj() {
      this->deviceIndexGamepad = i2c.znajdzCzujnik("dzojstik");
      this->deviceIndexKeypad = i2c.znajdzCzujnik("klawiatura-numeryczna");

      JsonArray & file = katalog::laduj("/config/menu.json");
      String optionName = file[0];

      if (this->version == "8") {
        this->populate(file[1]);
      } else {
        i2c.kanaly[0].porty[1].screen.populate(file[1]);
      }

      for (int i=this->aktywnaWarstwa+1; i<8; i++) {
        this->segments[i] = "";
      }
      this->segments[0] = optionName;
      
      for (int i=0; i<=indeksOstatniegoProgramu; i++) {
        if (programy[i]->active) {
          programy[i]->inicjuj();
        }
      }

      this->addressPath = this->getAddressPath();
      this->segmentsPath = this->getSegmentsPath();
    }

    void close() {
      this->segments[this->aktywnaWarstwa] = "";
      this->address[this->aktywnaWarstwa-1] = NULL;
      this->aktywnaWarstwa--;
      this->changed = true;
      
      if (aktywnyProgram != -1) {
        if (this->aktywnaWarstwa < programy[aktywnyProgram]->warstwaMenu) {
          agent.zamknijProgram();
        }
      }
    }
    
    void update() {
      ekran & screen = i2c.kanaly[0].porty[0].screen;

      if (screen.textScroll != 0) {
        screen.needsRefresh = true;
      }

      if (this->deviceIndexGamepad > -1) {
        if (i2c.czujniki[this->deviceIndexGamepad]->shortPress && this->mainMenuHovered()) {
          this->changed = false;

          if (screen.closeButtonHovered) {
            if (this->aktywnaWarstwa > 0) {
              this->close();
            }
          } else {
            if (screen.pathSegmentHovered > -1) {
              this->aktywnaWarstwa = screen.pathSegmentHovered;
              for (int i=this->aktywnaWarstwa+1; i < 8; i++) {
                this->segments[i] = "";
                this->address[i-1] = NULL;
                this->changed = true;
              }
              if (this->aktywnaWarstwa < programy[aktywnyProgram]->warstwaMenu) {
                agent.zamknijProgram();
              }
            }
          }

          if (this->changed) {
            this->addressPath = this->getAddressPath();
            this->segmentsPath = this->getSegmentsPath();
          }
        }
      }
    }

    JsonArray & getElement() {
      JsonArray & file = katalog::laduj("/config/menu.json");
      JsonArray * node = &file;
      for (int i=0; i<this->aktywnaWarstwa; i++) {
        JsonArray & children = (*node)[1];
        JsonArray & next = children[address[i]];
        node = &next;
      }
      return *node;
    }

    void selectOption(int index) {
      this->aktywnaWarstwa++;
      this->address[this->aktywnaWarstwa-1] = index;
      JsonArray & element = this->getElement();
      String optionName = element[0];
      this->segments[this->aktywnaWarstwa] = optionName;

      if (this->version == "8") {
        this->populate(element[1]);
      } else {
        ekran & screen = i2c.kanaly[0].porty[1].screen;
        screen.populate(element[1]);
      }

      JsonArray & command = element[2];
      if (command.size()) {
        int programIndex = agent.wykonaj(command);
        programy[programIndex]->warstwaMenu = this->aktywnaWarstwa + 1;
      }
    }

    void reactToGamepadAction() {
      przestrzen & surface = przestrzenie[0];
      JsonArray & element = this->getElement();

      if (surface.pointerPort == 0 && this->show && this->changed) {
        if (this->version == "8") {
          this->populate(element[1]);
          JsonArray & komenda = element[2];
          if (komenda.size()) {
            int indeksProgramu = agent.wykonaj(komenda);
            programy[indeksProgramu]->warstwaMenu = this->aktywnaWarstwa + 1;
          }
        } else {
          i2c.kanaly[0].porty[1].screen.populate(element[1]);
        }
      }

      if (surface.pointerPort > 0) {
        int index;
        if (this->version == "8") {
          index = surface.pointerPort;
        } else {
          ekran & screen = i2c.kanaly[0].porty[1].screen;
          index = screen.lineHovered;
        }
        
        if (element[1].size() > 0 && index < element[1].size()) {
          animacja.inicjuj(2);
          this->selectOption(index);
        }
      }

      this->addressPath = this->getAddressPath();
      this->segmentsPath = this->getSegmentsPath();
    }

    void reactToKeypadAction() {
      JsonArray & element = this->getElement();
      int index;
      char button = i2c.czujniki[this->deviceIndexKeypad]->buttonPressed;

      if (button != '*' && button != '#') {
        int index = button - 48;
        if (element[1].size() > 0 && index < element[1].size()) {
          this->selectOption(index);
        }
      }

      if (button == '*') {
        if (this->aktywnaWarstwa > 0) {
          this->close();
        }
      }

      this->addressPath = this->getAddressPath();
      this->segmentsPath = this->getSegmentsPath();
    }

    boolean mainMenuHovered() {
      przestrzen & surface = przestrzenie[0];
      boolean hovered = surface.pointerPort == 0 
        && (this->show || i2c.kanaly[0].porty[0].screen.closeButtonHovered);
      return hovered;
    }

    void aktualizuj() {
      przestrzen & surface = przestrzenie[0];

      if (surface.facingUp) {
        this->updatePointer();
        this->updateOptions();
        this->update();

        if (this->deviceIndexGamepad > -1 && i2c.czujniki[this->deviceIndexGamepad]->shortPress) {
          this->reactToGamepadAction();
        }

        if (this->deviceIndexKeypad > -1 && i2c.czujniki[this->deviceIndexKeypad]->shortPress) {
          this->reactToKeypadAction();
        }
      }
    }

    void rysuj() {
      przestrzen & surface = przestrzenie[0];
      ekran & screen = i2c.kanaly[0].porty[0].screen;

      if (this->show) {
        if (surface.pointerPort > 0) {
          screen.printLargestText(segments[aktywnaWarstwa]);
        } else {
          int cursorX = -1;
          int cursorY = -1;
          if (surface.pointerPort == 0) {
            cursorX = surface.getRelativeX();
            cursorY = surface.getRelativeY() - int(surface.getRelativeY() / screen.height);
          }
          screen.drawMenuAddress(addressPath);
          screen.drawMenuPath(segmentsPath, cursorX, cursorY);
        }
        
        this->drawOptions();
      }

      if (this->aktywnaWarstwa > 0) {
        screen.drawCloseButton();
      }
    }

    void drawOptions() {
      if (this->version == "8") {
        for (int port = 1; port < this->optionsCount; port++) {
          ekran & screen = i2c.kanaly[0].porty[port].screen;
          int amount = 2;
          screen.hasOptions = amount > 0;
          screen.optionsCount = amount;
          screen.minOffsetY = -(screen.optionsCount * 10) + screen.height - 1;
          screen.lines[0] = String(port);
          screen.lines[1] = this->options[port];
          screen.printLines();
        }
      } else {
        ekran & screen = i2c.kanaly[0].porty[1].screen;
        if (screen.hasOptions) {
          screen.printBoxes();
          screen.printLines();
          screen.drawScrollbar();
        }
      }
    }

    void populate(JsonArray & list) {
      przestrzen & surface = przestrzenie[0];
      if (list.size() > 0) {
        this->optionsCount = list.size();
        for (int i = 0; i < this->optionsCount; i++) {
          String optionName = list[i][0];
          this->options[i] = optionName;
        }
      } else {
        this->optionsCount = 0;
      }

      if (this->version == "8") {
        for (int port = 1; port < this->optionsCount; port++) {
          ekran & screen = i2c.kanaly[0].porty[port].screen;
          screen.lineSelected = -1;
          for (int i=0; i<=20; i++) {
            screen.lineScrollWidth[i] = 0;
          }
          screen.offsetY = 0;
        }
      }

      surface.refreshScreens();
    }

    interfejs() {
      for (int i=0; i<8; i++) {
        this->address[i] = i == 0 ? 0 : NULL;
      }
    }
};