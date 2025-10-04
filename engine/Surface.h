class Surface {
  public:
    int channel;
    
    int width = 128;
    int height = 256;

    int screenWidth = 128;
    int screenHeight = 32;
    int screensPerRow = (int)(this->width / this->screenWidth);
    int screensPerColumn = (int)(this->height / this->screenHeight);
    boolean screensDisabled = true;

    int orientationX = 1;
    int orientationY = 1;
    boolean facingUp = false;

    int pointerPositionX = this->width / 2;
    int pointerPositionY = this->height / 2;
    int pointerPreviousPositionX = 0;
    int pointerPreviousPositionY = 0;
    int pointerPreviousPort = 0;
    int pointerPort = 0;
    int pointerSpeed = 10;
    int screensCount = 0;

    String menuPath = "";
    String options[8];
    
    int optionsCount = 0;
    boolean hasOptions = false;

    int getRelativeX() {
      return this->pointerPositionX - (this->pointerPort % this->screensPerRow) * this->screenWidth;
    }

    int getRelativeY() {
      return this->pointerPositionY - (int)(this->pointerPort / this->screensPerRow) * this->screenHeight;
    }

    void init() {
      this->handleOrientationChange("up"); //accelerometer.orientation
      if (!this->facingUp) {
        this->turnScreens("off");
      }
      this->clear();
    }

    void handleOrientationChange(String orientation) {
      if (orientation == "up") {
        this->facingUp = this->orientationX == 1;
      } else {
        this->facingUp = this->orientationX == -1;
      }
      if (this->facingUp) {
        if (this->screensDisabled) {
          this->turnScreens("on");
        }
      } else {
        if (!this->screensDisabled) {
          this->turnScreens("off");
        }
      }
    }

    void updatePointer(double x, double y) {
      this->pointerPreviousPositionX = this->pointerPositionX;
      x *= this->orientationX;
      this->pointerPositionX -= (int)(x * this->pointerSpeed * this->orientationX);
      if (this->pointerPositionX < 0) {
        this->pointerPositionX = 0;
      }
      if (this->pointerPositionX > (this->width-1)) {
        this->pointerPositionX = (this->width-1);
      }
      this->pointerPreviousPositionY = this->pointerPositionY;
      this->pointerPositionY -= (int)(y * this->pointerSpeed * this->orientationY);
      if (this->pointerPositionY < 0) {
        this->pointerPositionY = 0;
      }
      if (this->pointerPositionY > (this->height-1)) {
        this->pointerPositionY = (this->height-1);
      }
      this->pointerPort = (int)(this->pointerPositionX / this->screenWidth) + (int)(this->pointerPositionY / this->screenHeight) * this->screensPerRow;
      channels[channel].ports[pointerPort].screen.needsRefresh = true;
    }

    boolean pointerPortChanged() {
      return this->pointerPreviousPort != this->pointerPort;
    }

    void tick() {
      if (this->pointerPortChanged()) {
        this->pointerPreviousPort = this->pointerPort;
      }
      if (accelerometer.orientationChanged) {
        this->handleOrientationChange(accelerometer.orientation);
      }
      if (this->facingUp) {
        if (gamepad.connected) {
          this->updatePointer(gamepad.axisX * this->orientationX, gamepad.axisY);
        }
      }
    }

    void turnScreens(String state) {
      for (int p=0; p<8; p++) {
        OLED & screen = channels[this->channel].ports[p].screen;
        screen.state = state;
        screen.needsRefresh = true;
      }
      this->screensDisabled = state == "off";
    }

  Surface() {}

  Surface(int width, int height, int screenWidth, int screenHeight, int orientationX, int orientationY, int channel) {
    this->width = width;
    this->height = height;
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->orientationX = orientationX;
    this->orientationY = orientationY;
    this->channel = channel;
  }

  int drawRectangle(int x, int y) {
    int height = 32;
    int port = (int)(y / height);
    OLED & screen = channels[this->channel].ports[port].screen;
    screen.clear();
    screen.drawRectangle(x, y - port * height);
    return port;
  }

  void drawRectangle2(int x, int y) {
    OLED & screen = channels[this->channel].ports[1].screen;
    screen.drawRectangle(x, y);
  }

  int drawPoint(int x, int y) {
    int height = 32;
    int port = (int)(y / height);
    OLED & screen = channels[this->channel].ports[port].screen;
    screen.drawPoint(x, y - port * height);
  }

  int drawCircle(int x, int y, int r) {
    int height = 32;
    int port = (int)(y / height);
    OLED & screen = channels[this->channel].ports[port].screen;
    for (int i=2; i<r; i++) {
      screen.ssd1306.drawCircle(x, y - port * height, i, SSD1306_WHITE);
    }
    screen.ssd1306.drawCircle(x, y - port * height, r, SSD1306_BLACK);
    screen.needsRefresh = true;
    return port;
  }

  void clear() {
    for (int i=0; i<8; i++) {
      OLED & screen = channels[this->channel].ports[i].screen;
      if (screen.needsRefresh) {
        screen.clear();
      }
    }
  }

  void drawBackButton(int port) {
    OLED & screen = channels[this->channel].ports[port].screen;
    screen.drawBackButton();
  }

  void drawPointer() {
    OLED & screen = channels[this->channel].ports[this->pointerPort].screen;
    screen.drawPointer(this->getRelativeX(), this->getRelativeY(), "circle");
  }

  void drawLine(int x1, int y1, int x2, int y2) {
    int height = 32;
    int port1 = (int)(y1 / height);
    int port2 = (int)(y2 / height);
    OLED & screen1 = channels[this->channel].ports[port1].screen;
    OLED & screen2 = channels[this->channel].ports[port2].screen;
    if (port1 == port2) {
      screen2.drawLine(x1, y1, x2, y2);
    } else {
      screen1.drawPoint(x1, y1 - port1 * height);
      screen2.drawPoint(x2, y2 - port2 * height);
    }
  }

  void drawMenuPath(int port) {
    OLED & screen = channels[this->channel].ports[port].screen;
    int cursorX = -1;
    int cursorY = -1;
    if (this->pointerPort == 0) {
      cursorX = this->getRelativeX();
      cursorY = this->getRelativeY() - int(this->getRelativeY() / screen.height);
    }
    screen.drawPath(this->menuPath, cursorX, cursorY);
  }

  void drawMenuOptions() {
    if (version == "8") {
      this->populateTick();
    } else {
      this->drawOptions(1);
    }
  }

  void drawOptions(int port) {
    OLED & screen = channels[this->channel].ports[port].screen;
    if (screen.hasOptions) {
      screen.printBoxes();
      screen.printLines();
      screen.drawScrollbar();
    }
  }

  String convertIntToString(int number) {
    String output = "";
    if (number == 0) output = "0";
    if (number == 1) output = "1";
    if (number == 2) output = "2";
    if (number == 3) output = "3";
    if (number == 4) output = "4";
    if (number == 5) output = "5";
    if (number == 6) output = "6";
    if (number == 7) output = "7";
    return output;
  }

  void populateInit(JsonArray & list) {
    if (list.size() > 0) {
      this->optionsCount = list.size();
      for (int i = 0; i < this->optionsCount; i++) {
        String optionName = list[i][0];
        String strNumber = this->convertIntToString(i);
        this->options[i] = strNumber + " " + optionName;
      }
    } else {
      this->optionsCount = 0;
    }
    this->hasOptions = this->optionsCount > 0;
  }

  void populateTick() {
    if (version == "8") {
      for (int port = 1; port < this->optionsCount; port++) {
        OLED & screen = channels[channel].ports[port].screen;
        screen.printText(this->options[port]);
      }
    }
  }

  static Surface * createFromConfigFile(JsonObject & configSurface) {
    int width = configSurface["width"];
    int height = configSurface["height"];
    int screenWidth = configSurface["screenWidth"];
    int screenHeight = configSurface["screenHeight"];
    int orientationX = configSurface["orientationX"];
    int orientationY = configSurface["orientationY"];
    int channel = configSurface["channel"];
    return new Surface(width, height, screenWidth, screenHeight, orientationX, orientationY, channel);
  }

  static int countChannels(JsonArray & configSurfaces) {
    int channelsUsed[8];
    for (int i=0; i<8; i++) {
      channelsUsed[i] = -1;
    }
    for (int i=0; i<surfacesCount; i++) {
      int channel = configSurfaces[i]["channel"];
      boolean found = false;
      for (int j=0; j<8; j++) {
        if (channelsUsed[j] == channel) {
          found = true;
        }
      }
      if (!found) {
        int index = 0;
        for (int j=0; j<8; j++) {
          if (channelsUsed[j] >= 0) {
            index++;
          }
        }
        channelsUsed[index] = channel;
      }
    }
    int countChannels = 0;
    for (int i=0; i<8; i++) {
      if (channelsUsed[i] >= 0) {
        countChannels++;
      }
    }
    return countChannels;
  }

  void countScreens() {
    this->screensCount = 0;
    for (int p=0; p<8; p++) {
      if (channels[channel].ports[p].screen.connected) {
        this->screensCount++;
      }
    }
  }

  void drawText(int port, String text) {
    channels[channel].ports[port].screen.printText(text);
  }

  void refreshScreens() {
    for (int i=0; i<8; i++) {
      channels[channel].ports[i].screen.needsRefresh = true;
    }
  }
};
