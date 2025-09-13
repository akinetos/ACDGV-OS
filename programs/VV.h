class VV:public Program {
  public:
    float cRe;
    float cIm;

    float xOffset = 0;
    float yOffset = 0;

    float scale = 200;
    int precision = 100;

    void init() {
      this->cRe = 0.22;
      this->cIm = 0.52;
      this->initialised = true;
    }

    void tick() {
      Surface & s8x1 = surfaces[0];

      if (s8x1.facingUp) {
        for (int i=0; i<8; i++) {
          channels[0].ports[i].screen.needsRefresh = true;
        }

        s8x1.clear();

        float newZre = 0;
        float newZim = 0;

        float offsetRe = accelerometer.x / this->precision;
        float offsetIm = accelerometer.y / this->precision;

        int x = 0;
        int y = 0;

        int previousX = 0;
        int previousY = 0;

        for (int i=1; i<200 && ((newZre*newZre + newZim*newZim) < 4); i++) {
          if (gamepad.axisX < -0.01 || gamepad.axisX > 0.01) {
            xOffset += gamepad.axisX / 10;
          }

          if (gamepad.axisY < -0.01 || gamepad.axisY > 0.01) {
            yOffset -= gamepad.axisY / 10;
          }

          x = s8x1.width / 2 + (int)(newZre * this->scale) + xOffset;
          y = s8x1.height / 2 + (int)(newZim * this->scale) + yOffset;

          if (x >=0 && x < s8x1.width && y >= 0 && y < s8x1.height) {
            if (this->option == 0) {
              s8x1.drawPoint(x, y);
            }
            if (this->option == 1) {
              s8x1.drawLine(previousX, previousY, x, y);
            }
          }

          float newerZre = newZre * newZre - newZim * newZim + this->cRe + offsetRe;
          float newerZim = newZre * newZim * 2 + this->cIm + offsetIm;
          newZre = newerZre;
          newZim = newerZim;
          previousX = x;
          previousY = y;
        }

        OLED & screen = channels[0].ports[7].screen;
        screen.ssd1306.setCursor(0, 0);
        screen.ssd1306.print("re: " + (String)(this->cRe + offsetRe));
        screen.ssd1306.setCursor(0, 10);
        screen.ssd1306.print("im: " + (String)(this->cIm + offsetIm));
        screen.needsRefresh = true;
      }
    }

  VV() {}
};
