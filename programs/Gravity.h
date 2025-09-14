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

        if (this->bounceX < -0.01 || this->bounceX > 0.01) {
          this->x += this->bounceX;
          this->bounceX *= 0.99;
        } else {
          this->x += tiltX;
        }

        if (this->bounceY < -0.01 || this->bounceY > 0.01) {
          this->y += this->bounceY;
          this->bounceY *= 0.99;
        } else {
          this->y += tiltY;
        }

        if (this->x < 0 || this->x > this->maxX) {
          this->bounceX = -tiltX;
        }

        if (this->y < 0 || this->y > this->maxY) {
          this->bounceY = -tiltY;
        }
      }

      void clearPreviousPort() {
        Surface & surface = surfaces[0];
        if (this->port != this->previousPort && this->previousPort != -1) {
          OLED & screen = channels[surface.channel].ports[this->previousPort].screen;
          screen.clear();
        }
        this->previousPort = this->port;
      }

      void init() {
        this->initialised = true;
      }

      void tick() {
        Surface & s8x1 = surfaces[0];
        this->port = (int)(this->y / 32);
        if (this->port < 8) {
          channels[s8x1.channel].ports[port].screen.clear();
          s8x1.fillCircle(this->x, this->y, 10);
          if (this->port != this->previousPort) {
            this->clearPreviousPort();
          }
        }
        this->moveLine();
      }

    Gravity() {}
};
