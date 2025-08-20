class VV:public Program {
  public:
    float cRe;
    float cIm;
    float scale;
    int lastX = 64;
    int lastY = 32;
    int precision = 100;
    float xOffset = 0;
    float yOffset = 0;

    void init() {
      this->cRe = 0.22;
      this->cIm = 0.52;
      this->scale = 50;
      this->initialised = true;
    }

    void tick() {
      Surface & s2x1 = surfaces[0];
      Surface & s8x1 = surfaces[1];

      float newZre = 0;
      float newZim = 0;
      float offsetX = accelerometer.x / this->precision;
      float offsetY = accelerometer.y / this->precision;

      for (int i=1; i<200 && ((newZre*newZre + newZim*newZim) < 4); i++) {
        if (i < 200) {
          this->scale = 200;
          int x = 0;
          int y = 0;

          if (surfacesCount > 1) {
            if (surfaces[1].facingUp) {
              if (gamepad.axisX < -0.1 || gamepad.axisX > 0.1) {
                xOffset -= gamepad.axisX / 10;
              }
              if (gamepad.axisY < -0.1 || gamepad.axisY > 0.1) {
                yOffset -= gamepad.axisY / 10;
              }
              x = surfaces[1].width / 2 + (int)(newZre * this->scale) + xOffset;
              y = surfaces[1].height / 2 + (int)(newZim * this->scale) + yOffset;
              if (x >=0 && x < surfaces[1].width && y >= 0 && y < surfaces[1].height) {
                if (this->option == 0) {
                  surfaces[1].drawPoint(x, y);
                }
                if (this->option == 1) {
                  surfaces[1].drawLine(lastX, lastY, x, y);
                }
              }
            }
          }

          if (s2x1.facingUp) {
            if (gamepad.axisX < -0.1 || gamepad.axisX > 0.1) {
              xOffset += gamepad.axisX / 20;
            }
            if (gamepad.axisY < -0.1 || gamepad.axisY > 0.1) {
              yOffset -= gamepad.axisY / 20;
            }
            x = s2x1.width / 2 + (int)(newZre * this->scale) + xOffset;
            y = s2x1.height / 2 + (int)(newZim * this->scale) + yOffset;
            if (x >=0 && x < s2x1.width && y >= 0 && y < s2x1.height) {
              if (this->option == 0) {
                s2x1.drawPoint(x, y);
              }
              if (this->option == 1) {
                s2x1.drawLine(lastX, lastY, x, y);
              }
            }
          }

          float newerZre = newZre * newZre - newZim * newZim + this->cRe + offsetX;
          float newerZim = newZre * newZim * 2 + this->cIm + offsetY;
          newZre = newerZre;
          newZim = newerZim;
          this->lastX = x;
          this->lastY = y;
        } else {
          break;
        }
      }

      if (s8x1.facingUp) {
        OLED & screen = channels[s8x1.channel].ports[7].screen;
        screen.ssd1306.setCursor(0, 0);
        screen.ssd1306.print("re: " + (String)(this->cRe + offsetX));
        screen.ssd1306.setCursor(0, 10);
        screen.ssd1306.print("im: " + (String)(this->cIm + offsetY));
        screen.needsRefresh = true;
      }

      if (s2x1.facingUp) {
        Port & p01 = channels[0].ports[1];
        p01.screen.ssd1306.setTextColor(SSD1306_WHITE);
        p01.screen.ssd1306.setCursor(0, 0);
        p01.screen.ssd1306.print("re: " + (String)(this->cRe + offsetX));
        p01.screen.ssd1306.setCursor(0, 10);
        p01.screen.ssd1306.print("im: " + (String)(this->cIm + offsetY));
        p01.screen.needsRefresh = true;
      }
    }

  VV() {}
};
