class Pixel {
  public:
    int x;
    int y;
    
  Pixel() {}

  Pixel(int x, int y) {
    this->x = x;
    this->y = y;
  }
};

class FallEffect {
  public:
    boolean active = false;
    Pixel pixels[500];
    int counter = 0;
    int p = 0;

    void init() {
      this->counter = 0;
      this->p = 0;
      for (int y=0; y<256; y+=5) {
        for (int x=0; x<128; x+=5) {
          boolean white = surfaces[0].getPixel(x,y);
          if (white && p<500) {
            this->pixels[p] = Pixel(x,y);
            p++;
          }
        }
      }
    }

    void tick() {
      for (int i=0; i<p; i++) {
        int weight = this->counter * this->counter;
        this->pixels[i].y += weight;
      }
      for (int i=0; i<8; i++) {
        channels[0].ports[i].screen.needsRefresh = true;
      }
    }

    void draw() {
      for (int i=0; i<p; i++) {
        if (this->pixels[i].y < 256) {
          int port = surfaces[0].drawPoint(this->pixels[i].x, this->pixels[i].y);
        }
      }
      this->counter++;
      if (this->counter > 16) {
        przejscie = false;
        this->active = false;
        this->counter = 0;
      }
    }

  FallEffect() {}
};
