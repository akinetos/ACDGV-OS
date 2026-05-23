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
    Pixel pixels[300];
    int counter = 0;
    int p = 0;

    void init() {
      this->counter = 0;
      this->p = 0;
      for (int y=0; y<96; y+=4) {
        for (int x=0; x<128; x+=4) {
          boolean white = surfaces[0].getPixel(x,y);
          if (white && p<300) {
            this->pixels[p] = Pixel(x,y);
            p++;
          }
        }
      }
    }

    void prepare() {}

    void tick() {}

    void draw() {
      for (int i=0; i<p; i++) {
        this->pixels[i].y++;
      }
      for (int i=0; i<p; i++) {
        surfaces[0].drawPoint(this->pixels[i].x, this->pixels[i].y);
      }
      this->counter++;
      if (this->counter > 100) {
        przejscie = false;
        this->active = false;
      }
    }

  FallEffect() {}
};
