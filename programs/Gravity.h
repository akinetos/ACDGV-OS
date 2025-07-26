class Gravity:public Program {
    public:
      int y = 0;
      int maxY = 0;
      int bounceY = 0;
      int port = -1;
      int previousPort = -1;
      int option = 5;
      int startedTime;

      void moveLine() {
        int tiltY = (int)(accelerometer.y * this->option);
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
        Surface & surface = surfaces[1];
        if (this->port != this->previousPort && this->previousPort != -1) {
          OLED & screen = channels[surface.channel].ports[this->previousPort].screen;
          screen.clear();
          screen.needsRefresh = true;
        }
        this->previousPort = this->port;
      }

      void init() {
        this->startedTime = millis();
        this->y = 0;
      }

      void tick() {
        Port & p05 = channels[0].ports[5];
        
        Surface & s2x1 = surfaces[0];
        Surface & s8x1 = surfaces[1];

        if (p05.re.changed) {
          this->setOption(p05.re.position);
        }
        
        if (s8x1.facingUp) {
          this->maxY = 255;
          this->port = s8x1.drawRectangle(0, this->y);
          this->clearPreviousPort();
        }

        if (s2x1.facingUp) {
          this->maxY = 32;
          s2x1.drawRectangle2(0, this->y);
        }
        
        this->moveLine();
      }

      void setOption(int option) {
        this->option = option;
      }

      void becameActive() {}

    Gravity() {}
};
