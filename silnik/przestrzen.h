class przestrzen {
  public:
    int channel;
    
    int width = 128;
    int height = 256;

    int screenWidth = 128;
    int screenHeight = 32;
    int screensPerRow = (int)(this->width / this->screenWidth);
    int screensPerColumn = (int)(this->height / this->screenHeight);
    boolean screensDisabled = true;

    int orientationX = -1;
    int orientationY = 1;
    boolean facingUp = false;

    int pointerPositionX = this->width / 2;
    int pointerPositionY = this->height / 2;
    int pointerPreviousPositionX = 0;
    int pointerPreviousPositionY = 0;
    int pointerPreviousPort = 0;
    int pointerPort = 0;
    int pointerSpeed = 10;
    int animatePointerTime = 0;
    int screensCount = 0;
    
    boolean showPointer = true;

    int deviceIndexGamepad = -1;
    int deviceIndexAccelerometer = -1;

    int getRelativeX() {
      return this->pointerPositionX - (this->pointerPort % this->screensPerRow) * this->screenWidth;
    }

    int getRelativeY() {
      return this->pointerPositionY - (int)(this->pointerPort / this->screensPerRow) * this->screenHeight;
    }

    void inicjuj(JsonObject & config) {
      this->deviceIndexGamepad = i2c.znajdzCzujnik("dzojstik");
      this->deviceIndexAccelerometer = i2c.znajdzCzujnik("czujnik-nachylenia");

      int width = config["width"];
      int height = config["height"];
      int screenWidth = config["screenWidth"];
      int orientationY = config["orientationY"];
      int channel = config["channel"];

      this->width = width;
      this->height = height;
      this->screenWidth = screenWidth;
      this->screenHeight = screenHeight;
      this->channel = channel;

      if (this->deviceIndexAccelerometer > -1)
        this->handleOrientationChange(czujniki[this->deviceIndexAccelerometer]->orientation);
      else
        this->handleOrientationChange("up");

      if (!this->facingUp)
        this->turnScreens("off");

      this->countScreens();
      this->czysc();
    }

    void handleOrientationChange(String orientation) {
      if (orientation == "up") {
        this->facingUp = this->orientationX == 1;
      } else {
        this->facingUp = this->orientationX == -1;
      }

      //DO ZROBIENIA: tymczasowe rozwiązanie
      this->facingUp = true;
      
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
      x *= this->orientationX;
      y *= this->orientationY;
      
      this->pointerPreviousPositionX = this->pointerPositionX;
      this->pointerPositionX += (int)(x * this->pointerSpeed);
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
      kanaly[channel].porty[pointerPort].screen.needsRefresh = true;
    }

    boolean pointerPortChanged() {
      return this->pointerPreviousPort != this->pointerPort;
    }

    void aktualizuj() {
      if (this->deviceIndexGamepad > -1)
        if (this->facingUp && this->showPointer)
          this->updatePointer(czujniki[this->deviceIndexGamepad]->readNumber("x"), czujniki[this->deviceIndexGamepad]->readNumber("y"));

      if (this->deviceIndexAccelerometer > -1)
        if (czujniki[this->deviceIndexAccelerometer]->orientationChanged)
          this->handleOrientationChange(czujniki[this->deviceIndexAccelerometer]->orientation);

      if (this->deviceIndexGamepad > -1)
        if (czujniki[this->deviceIndexGamepad]->shortPress || czujniki[this->deviceIndexGamepad]->longPress)
          this->animatePointerTime = millis();
    }

    void turnScreens(String state) {
      for (int p=0; p<8; p++) {
        OLED & screen = kanaly[this->channel].porty[p].screen;
        screen.state = state;
        screen.needsRefresh = true;
      }
      this->screensDisabled = state == "off";
    }

  przestrzen() {}

  int drawRectangle(int x, int y) {
    int height = 32;
    int port = (int)(y / height);
    OLED & screen = kanaly[this->channel].porty[port].screen;
    screen.clear();
    screen.drawRectangle(x, y - port * height);
    return port;
  }

  int drawPoint(int x, int y) {
    int height = 32;
    int port = (int)(y / height);
    OLED & screen = kanaly[this->channel].porty[port].screen;
    screen.drawPoint(x, y - port * height);
    return port;
  }

  int drawCircle(int x, int y, int r) {
    int height = 32;
    int port = (int)(y / height);
    OLED & screen = kanaly[this->channel].porty[port].screen;
    for (int i=2; i<r; i++) {
      screen.ssd1306.drawCircle(x, y - port * height, i, SSD1306_WHITE);
    }
    screen.ssd1306.drawCircle(x, y - port * height, r, SSD1306_BLACK);
    screen.needsRefresh = true;
    return port;
  }

  void czysc() {
    for (int i=0; i<8; i++) {
      OLED & screen = kanaly[this->channel].porty[i].screen;
      if (screen.needsRefresh) {
        screen.clear();
      }
    }
  }

  void drawPointer() {
    OLED & screen = kanaly[this->channel].porty[this->pointerPort].screen;
    String mode = "circle";
    if (this->animatePointerTime + 1000 > millis()) {
      mode = "rect";
    }
    screen.drawPointer(this->getRelativeX(), this->getRelativeY(), mode);
    if (this->pointerPortChanged()) {
      this->pointerPreviousPort = this->pointerPort;
    }
  }

  void drawLine(int x1, int y1, int x2, int y2) {
    int height = 32;
    int port1 = (int)(y1 / height);
    int port2 = (int)(y2 / height);
    OLED & screen1 = kanaly[this->channel].porty[port1].screen;
    OLED & screen2 = kanaly[this->channel].porty[port2].screen;
    if (port1 == port2) {
      screen2.drawLine(x1, y1 - port1 * height, x2, y2 - port1 * height);
    } else {
      screen1.drawPoint(x1, y1 - port1 * height);
      screen2.drawPoint(x2, y2 - port2 * height);
    }
  }

  void countScreens() {
    this->screensCount = 0;
    for (int p=0; p<8; p++) {
      if (kanaly[channel].porty[p].screen.connected) {
        this->screensCount++;
      }
    }
  }

  void drawText(int port, String text) {
    kanaly[channel].porty[port].screen.printText(text);
  }

  void refreshScreens() {
    for (int i=0; i<8; i++) {
      kanaly[channel].porty[i].screen.needsRefresh = true;
    }
  }

  void display() {
    for (int i=0; i<8; i++) {
      kanaly[channel].porty[i].display();
    }
  }

  void rysuj() {
    if (this->showPointer) {
      this->drawPointer();
    }

    this->display();
  }

  void drawBitmap(const unsigned char* image, int port, int offsetY) {
    kanaly[channel].porty[port].screen.ssd1306.drawBitmap(
      0, offsetY,
      image,
      128, 64,
      SSD1306_WHITE
    );
    kanaly[channel].porty[port].screen.needsRefresh = true;
  }

  boolean getPixel(int x, int y) {
    int height = 32;
    int port = (int)(y / height);
    boolean white = kanaly[0].porty[port].screen.ssd1306.getPixel(x, y - port * height);
    return white;
  }
};
