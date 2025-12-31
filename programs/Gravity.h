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

      int gestureTimestamp = 0;
      String gestureDirection = "";

      void tick() {
        int speed = (this->option + 1) * 10;
        int tiltX = 0;
        int tiltY = 0;

        tiltX += (int)(-accelerometer.x * speed);
        tiltY += (int)(accelerometer.y * speed);

        if (gd.changed || ((this->gestureTimestamp + 1000) > millis())) {
          if (gd.changed) {
            this->gestureTimestamp = millis();
            this->gestureDirection = gd.gesture;
          }
          
          if (this->gestureDirection == "gora") tiltY -= 1;
          if (this->gestureDirection == "dol") tiltY += 1;
          if (this->gestureDirection == "lewo") tiltX -= 1;
          if (this->gestureDirection == "prawo") tiltX += 1;
        }

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
          if (this->x < 0) this->x = 0;
          if (this->x > this->maxX) this->x = this->maxX;
          this->bounceX = -tiltX;
        }

        if (this->y < 0 || this->y > this->maxY) {
          if (this->y < 0) this->y = 0;
          if (this->y > this->maxY) this->y = this->maxY;
          this->bounceY = -tiltY;
        }

        this->port = (int)(this->y / 32);
        channels[0].ports[port].screen.needsRefresh = true;
      }

      void init() {
        if (version == "3") maxY = 95;
        if (version == "8") maxY = 255;
        this->initialised = true;
      }

      void draw() {
        Surface * surface = & surfaces[0];
        surface->drawCircle(this->x, this->y, 10);
        if (this->port != this->previousPort) {
          if (this->port != this->previousPort && this->previousPort != -1) {
            OLED & screen = channels[surface->channel].ports[this->previousPort].screen;
            screen.clear();
          }
          this->previousPort = this->port;
        }
      }

    Gravity() {}
};
