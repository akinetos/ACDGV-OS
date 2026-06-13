class Transition {
  public:
    boolean active = false;
    Pixel pixels[500];
    int counter = 0;
    int p = 0;

    void init() {
      this->counter = 0;
      this->p = 0;
      if (transitionType == 1) {
        for (int y=32; y<256; y+=5) {
          for (int x=0; x<128; x+=5) {
            boolean white = surfaces[0].getPixel(x,y);
            if (white && p<500) {
              this->pixels[p] = Pixel(x,y);
              p++;
            }
          }
        }
      }
      if (transitionType == 2) {
        int minY = surfaces[0].pointerPort * 32;
        int maxY = (surfaces[0].pointerPort + 1) * 32;
        for (int y=minY; y<maxY; y+=2) {
          for (int x=0; x<128; x+=2) {
            boolean white = surfaces[0].getPixel(x,y);
            if (white && p<500) {
              this->pixels[p] = Pixel(x,y);
              p++;
            }
          }
        }
      }
      this->active = true;
    }

    void tick() {
      if (this->active) {
        for (int i=0; i<p; i++) {
          int weight = this->counter * this->counter;
          if (transitionType == 1) {
            if (this->pixels[i].y + weight < 256) {
              this->pixels[i].y += weight;
            } else {
              this->pixels[i].y = 256;
            }
          }
          if (transitionType == 2) {
            if (this->pixels[i].y - weight > 0) {
              this->pixels[i].y -= weight;
            } else {
              this->pixels[i].y = 0;
            }
          }
        }
        for (int i=0; i<8; i++) {
          channels[0].ports[i].screen.needsRefresh = true;
        }
      }
    }

    void draw() {
      if (this->active) {
        for (int i=0; i<p; i++) {
          if (this->pixels[i].x > 0 
            && this->pixels[i].x < 128
            && this->pixels[i].y > 32
            && this->pixels[i].y < 256
          ) {
            int port = surfaces[0].drawPoint(this->pixels[i].x, this->pixels[i].y);
          }
        }
        this->counter++;
        if (this->counter > 15) {
          this->active = false;
          this->counter = 0;
        }
      }
    }

  Transition() {}
};
