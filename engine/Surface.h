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
    String menuAddress = "";
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
      this->handleOrientationChange(devices[0]->orientation);
      if (!this->facingUp) {
        this->turnScreens("off");
      }
      this->countScreens();
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
      this->pointerPositionX += (int)(x * this->pointerSpeed * this->orientationX);
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
      if (devices[0]->orientationChanged) { //accelerometer
        this->handleOrientationChange(devices[0]->orientation);
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

  void drawCloseButton(int port) {
    OLED & screen = channels[this->channel].ports[port].screen;
    screen.drawCloseButton();
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
      screen2.drawLine(x1, y1 - port1 * height, x2, y2 - port1 * height);
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
    screen.drawMenuAddress(this->menuAddress);
    screen.drawMenuPath(this->menuPath, cursorX, cursorY);
  }

  void drawMenuOptions() {
    if (version == "8") {
      for (int port = 1; port < this->optionsCount; port++) {
        OLED & screen = channels[channel].ports[port].screen;
        int amount = 2;
        screen.hasOptions = amount > 0;
        screen.optionsCount = amount;
        screen.minOffsetY = -(screen.optionsCount * 10) + screen.height - 1;
        screen.lines[0] = String(port);
        screen.lines[1] = this->options[port];
        screen.printLines();
      }
    } else {
      OLED & screen = channels[this->channel].ports[1].screen;
      if (screen.hasOptions) {
        screen.printBoxes();
        screen.printLines();
        screen.drawScrollbar();
      }
    }
  }

  void populate(JsonArray & list) {
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
        OLED & screen = channels[channel].ports[port].screen;
        screen.lineSelected = -1;
        for (int i=0; i<=20; i++) {
          screen.lineScrollWidth[i] = 0;
        }
        screen.offsetY = 0;
      }
    }

    this->refreshScreens();
  }

  static Surface * create(JsonObject & config) {
    int width = config["width"];
    int height = config["height"];
    int screenWidth = config["screenWidth"];
    int screenHeight = config["screenHeight"];
    int orientationX = config["orientationX"];
    int orientationY = config["orientationY"];
    int channel = config["channel"];
    return new Surface(width, height, screenWidth, screenHeight, orientationX, orientationY, channel);
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
