const int pointsCount = 1000;

class VV:public Program {
  public:
    float cRe;
    float cIm;

    float xOffset = 0;
    float yOffset = -50;

    float offsetRe = 0;
    float offsetIm = 0;

    float scale = 200;
    int precision = 100;
    
    int points[pointsCount * 2];

    boolean move = false;

    boolean screensUpdated[8];

    void init() {
      this->cRe = 0.22;
      this->cIm = 0.52;
      for (int port=0; port<8; port++) {
        this->screensUpdated[port] = false;
      }
      this->initialised = true;
    }

    void compute() {
      Surface * surface = & surfaces[0];

      float newZre = 0;
      float newZim = 0;

      this->points[0] = 0;
      this->points[1] = 0;

      this->offsetRe = accelerometer.x / this->precision;
      this->offsetIm = accelerometer.y / this->precision;

      if (this->move) {
        if (devices[4]->x < -0.01 || devices[4]->x > 0.01) {
          xOffset += devices[4]->x * 10;
        }
        if (devices[4]->y < -0.01 || devices[4]->y > 0.01) {
          yOffset -= devices[4]->y * 10;
        }
      }

      for (int port=0; port<8; port++) {
        if (this->screensUpdated[port]) {
          channels[0].ports[port].screen.needsRefresh = true;
        }
      }

      for (int port=0; port<8; port++) {
        this->screensUpdated[port] = false;
      }
      
      for (int i=1; i<(pointsCount-1) && ((newZre*newZre + newZim*newZim) < 4); i++) {
        this->points[i*2] = surface->width / 2 + (int)(newZre * this->scale) + xOffset;
        this->points[i*2+1] = surface->height / 2 + (int)(newZim * this->scale) + yOffset;

        int port = (int)(this->points[i*2+1] / 32);
        if (port >=0) {
          if ((version == "3" && port < 3) || (version == "8" && port < 8)) {
            channels[0].ports[port].screen.needsRefresh = true;
            this->screensUpdated[port] = true;
          }
        }

        float newerZre = newZre * newZre - newZim * newZim + this->cRe + this->offsetRe;
        float newerZim = newZre * newZim * 2 + this->cIm + this->offsetIm;
        newZre = newerZre;
        newZim = newerZim;
      }
    }

    void tick() {
      if (devices[4]->longPress) {
        this->move = !this->move;
        Surface * surface = & surfaces[0];
        if (this->move) {
          surface->showPointer = false;
          surface->showMenu = false;
        } else {
          surface->showPointer = true;
          surface->showMenu = true;
        }
      }

      this->compute();

      channels[0].ports[0].screen.needsRefresh = true;
      
      if (nfcDevice.message != "") {
        channels[0].ports[7].screen.needsRefresh = true;
      }
    }

    boolean isWithinRange(int i) {
      Surface * surface = & surfaces[0];
      return points[i*2] >=0 && points[i*2] < surface->width && points[i*2+1] >= 0 && points[i*2+1] < surface->height;
    }

    void draw() {
      Surface * surface = & surfaces[0];

      if (surface->facingUp) {
        if (this->option == 0) {
          for (int i=0; i<pointsCount; i++) {
            if (this->isWithinRange(i)) {
              surface->drawPoint(points[i*2], points[i*2+1]);
            }
          }

          if (nfcDevice.message != "") {
            OLED & screen = channels[0].ports[7].screen;
            screen.printText(nfcDevice.message);
          }
        }

        if (this->option == 1) {
          for (int i=0; i<(pointsCount-1); i++) {
            if (this->isWithinRange(i) && this->isWithinRange(i+1)) {
              surface->drawLine(points[i*2], points[i*2+1], points[(i+1)*2], points[(i+1)*2+1]);
            }
          }
        }

        if (this->option == 2) {
          action = "nfc read";
          this->option = 0;
        }

        if (this->option == 3) {
          String content = "[" + String(this->cRe + this->offsetRe) + "," + String(this->cIm + this->offsetIm) + "]";
          nfcDevice.content = content;
          action = "nfc write";
          this->option = 0;
        }

        if (devices[5]->shortPress) {
          char button = devices[5]->buttonPressed;
          if (button != '*' && button != '#') {
            int index = button - 48;
            if (index == 1) {
              String content = "[[2,5],[" + String(this->cRe + this->offsetRe) + "," + String(this->cIm + this->offsetIm) + "]]";
              nfcDevice.content = content;
              action = "nfc write";
            }
          }
        }

        this->drawProgress();
      }
    }

    void justActivated() {
      this->activatedTimestamp = millis();
      this->counter = 0;
      this->compute();
    }

    void drawProgress() {
      OLED & screen = channels[0].ports[0].screen;
      screen.ssd1306.setCursor(64, 4);
      screen.ssd1306.setTextColor(SSD1306_WHITE);
      screen.ssd1306.setTextWrap(false);
      if (this->move) {
        screen.ssd1306.print(String((int)this->xOffset) + "/" + String((int)this->yOffset));
      } else {
        screen.ssd1306.print(String(this->cRe + this->offsetRe) + "/" + String(this->cIm + this->offsetIm));
      }
    }

  VV() {}
};
