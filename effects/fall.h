class Pixel {
  public:
    int x;
    int y;
    
  Pixel() {
    this->x = 0;
    this->y = 0;
  }
};

class FallEffect {
  public:
    boolean active = false;
    Pixel pixels[10];

    void init() {}

    void prepare() {}

    void tick() {}

    void draw() {}

  FallEffect() {}
};
