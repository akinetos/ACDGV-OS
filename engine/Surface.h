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

    int getRelativeX() {
      return this->pointerPositionX - (this->pointerPort % this->screensPerRow) * this->screenWidth;
    }

    int getRelativeY() {
      return this->pointerPositionY - (int)(this->pointerPort / this->screensPerRow) * this->screenHeight;
    }

    void init() {
      this->handleOrientationChange(accelerometer.orientation);
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
      x *= this->orientationX * (-1);
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
        this->updatePointer(gamepad.axisX * this->orientationX, gamepad.axisY);
      }
    }

    void tick(String mode) {
      channels[this->channel].tick(mode);
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
    screen.clear();
    screen.drawRectangle(x, y);
  }

  int drawPoint(int x, int y) {
    int height = 32;
    int port = (int)(y / height);
    OLED & screen = channels[this->channel].ports[port].screen;
    screen.drawPoint(x, y - port * height);
  }

  int fillCircle(int x, int y, int r) {
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
    for (int i=0; i<=7; i++) {
      OLED & oled = channels[this->channel].ports[i].screen;
      oled.clear();
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

  void populateScreen(int port, JsonArray & json) {
    OLED & screen = channels[this->channel].ports[port].screen;
    screen.populate(json);
  }
};
