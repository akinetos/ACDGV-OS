const int pointsCount = 1000;

class VV:public Program {
  public:
    float cRe;
    float cIm;

    float xOffset = 0;
    float yOffset = 0;

    float offsetRe = 0;
    float offsetIm = 0;

    float scale = 200;
    int precision = 100;
    
    int points[pointsCount * 2];

    boolean move = true;

    boolean screensUpdated[8];

    void init() {
      this->cRe = 0.22;
      this->cIm = 0.52;
      this->initialised = true;
      this->move = true;
      for (int port=0; port<8; port++) {
        this->screensUpdated[port] = false;
      }
    }

    void update() {
      if (gamepad.buttonApressed()) {
        //this->move = !this->move;
      }

      if (this->move) {
        Surface & surface = surfaces[0];

        float newZre = 0;
        float newZim = 0;

        this->points[0] = 0;
        this->points[1] = 0;

        this->offsetRe = accelerometer.x / this->precision;
        this->offsetIm = accelerometer.y / this->precision;

        if (gamepad.axisX < -0.01 || gamepad.axisX > 0.01) {
          xOffset += gamepad.axisX * 10;
        }

        if (gamepad.axisY < -0.01 || gamepad.axisY > 0.01) {
          yOffset -= gamepad.axisY * 10;
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
          this->points[i*2] = surface.width / 2 + (int)(newZre * this->scale) + xOffset;
          this->points[i*2+1] = surface.height / 2 + (int)(newZim * this->scale) + yOffset;

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
    }

    boolean isWithinRange(int i) {
      Surface & surface = surfaces[0];
      return points[i*2] >=0 && points[i*2] < surface.width && points[i*2+1] >= 0 && points[i*2+1] < surface.height;
    }

    void tick() {
      Surface & surface = surfaces[0];

      if (surface.facingUp) {
        if (this->option == 0) {
          for (int i=0; i<pointsCount; i++) {
            if (this->isWithinRange(i)) {
              surface.drawPoint(points[i*2], points[i*2+1]);
            }
          }
        }

        if (this->option == 1) {
          for (int i=0; i<(pointsCount-1); i++) {
            if (this->isWithinRange(i) && this->isWithinRange(i+1)) {
              surface.drawLine(points[i*2], points[i*2+1], points[(i+1)*2], points[(i+1)*2+1]);
            }
          }
        }

        this->drawProgress();
      }
    }

    void justActivated() {
      this->activatedTimestamp = millis();
      this->counter = 0;
      this->move = true;
    }

    void drawProgress() {
      OLED & screen = channels[0].ports[0].screen;
      screen.ssd1306.setCursor(64, 4);
      screen.ssd1306.setTextColor(SSD1306_WHITE);
      screen.ssd1306.setTextWrap(false);
      screen.ssd1306.print(String(this->cRe + this->offsetRe) + "/" + String(this->cIm + this->offsetIm));
    }

  VV() {}
};
