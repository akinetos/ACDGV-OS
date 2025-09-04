class Gravity:public Program {
    public:
      int y = 0;
      int maxY = 0;
      int bounceY = 0;
      int port = -1;
      int previousPort = -1;

      void moveLine() {
        int tiltY = (int)(-accelerometer.y * (this->option + 1));
        this->y += tiltY;
        if (this->y < 0 || this->y > this->maxY) {
          this->bounceY = -tiltY;
        }
        if (this->bounceY != 0) {
          this->y += this->bounceY * 2;
          if (this->bounceY > 0) {
            this->bounceY--;
          } else {
            this->bounceY++;
          }
        }
      }

      void clearPreviousPort() {
        if (surfacesCount > 1) {
          Surface & surface = surfaces[1];
          if (this->port != this->previousPort && this->previousPort != -1) {
            OLED & screen = channels[surface.channel].ports[this->previousPort].screen;
            screen.clear();
            screen.needsRefresh = true;
          }
          this->previousPort = this->port;
        } else {
          Surface & surface = surfaces[0];
          if (this->port != this->previousPort && this->previousPort != -1) {
            OLED & screen = channels[surface.channel].ports[this->previousPort].screen;
            screen.clear();
            screen.needsRefresh = true;
          }
          this->previousPort = this->port;
        }
      }

      void init() {
        this->y = 0;
        this->initialised = true;
      }

      void tick() {
        if (surfacesCount > 1) {
          Surface & s2x1 = surfaces[0];
          Surface & s8x1 = surfaces[1];
          if (s8x1.facingUp) {
            this->maxY = 255;
            this->port = s8x1.drawRectangle(0, this->y);
            this->clearPreviousPort();
          }
          if (s2x1.facingUp) {
            this->maxY = 32;
            s2x1.drawRectangle2(0, this->y);
          }
        } else {
          Surface & s8x1 = surfaces[0];
          this->maxY = 255;
          this->port = s8x1.drawRectangle(0, this->y);
          this->clearPreviousPort();
        }
        
        this->moveLine();
      }

    Gravity() {}
};
