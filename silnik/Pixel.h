class Pixel {
  public:
    uint8_t x;
    uint8_t y;
    boolean widoczny;
    
  Pixel() {}

  Pixel(uint8_t x, uint8_t y) {
    this->x = x;
    this->y = y;
    this->widoczny = true;
  }
};