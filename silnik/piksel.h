class piksel {
  public:
    uint8_t x;
    uint8_t y;
    boolean widoczny;
    
  piksel() {}

  piksel(uint8_t x, uint8_t y) {
    this->x = x;
    this->y = y;
    this->widoczny = true;
  }
};