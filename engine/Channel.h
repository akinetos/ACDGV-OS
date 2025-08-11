#include "./Port.h";

class Channel {
  public:
    int number;
    Port ports[8];

    void init(int number) {
      this->number = number;
      for (int p=0; p<8; p++) {
        this->ports[p].number = p;
        this->ports[p].channel = this->number;
        this->ports[p].init();
      }
      i2c.deactivate(this->number);
    }

    void tick() {
      for (int p = 0; p < 8; p++) {
        this->ports[p].tick();
      }
    }

    void display() {
      for (int p = 0; p < 8; p++) {
        this->ports[p].display();
      }
    }
    
  Channel() {}
};
