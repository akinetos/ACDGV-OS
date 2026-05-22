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
    Pixel pixels[4];
    int counter = 0;

    void init() {
      this->counter = 0;
      pixels[0] = Pixel(1,1);
      pixels[1] = Pixel(3,3);
      pixels[2] = Pixel(5,5);
      pixels[3] = Pixel(7,7);
    }

    void prepare() {}

    void tick() {}

    void draw() {
      surfaces[0].drawPoint(this->counter, 100);
      surfaces[0].drawPoint(this->counter, 101);
      surfaces[0].drawPoint(this->counter, 102);
      this->counter++;
      if (this->counter > 100) {
        przejscie = false;
        this->active = false;
      }
    }

  FallEffect() {}
};
