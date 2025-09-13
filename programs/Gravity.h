class Gravity:public Program {
    public:
      int x = 64;
      int y = 128;

      int maxX = 127;
      int maxY = 255;

      int bounceX = 0;
      int bounceY = 0;

      int port = -1;
      int previousPort = -1;

      void moveLine() {
        int speed = (this->option + 1) * 2;
        int tiltX = (int)(-accelerometer.x * speed);
        int tiltY = (int)(-accelerometer.y * speed);

        this->x += tiltX;
        this->y += tiltY;

        if (this->x < 0 || this->x > this->maxX) {
          this->bounceX = -tiltX;
        }

        if (this->y < 0 || this->y > this->maxY) {
          this->bounceY = -tiltY;
        }

        if (this->bounceX != 0) {
          this->x += this->bounceX * 2;
          if (this->bounceX > 0) {
            this->bounceX--;
          } else {
            this->bounceX++;
          }
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
          this->port = (int)(this->y / 32);
          channels[0].ports[port].screen.clear();
          s8x1.fillCircle(this->x, this->y, 5);
          if (this->port != this->previousPort) {
            this->clearPreviousPort();
          }
        }
        
        this->moveLine();
      }

    Gravity() {}
};
