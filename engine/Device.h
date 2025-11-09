class Device {
  public:
    boolean connected;
    boolean changed;
    float axisY;

    Device();
    void virtual init(void);
    void virtual tick(void);
    boolean virtual buttonApressed() {
      return false;
    }

    double x;
    double y;
    int position = 0;
};

Device::Device() {}

void Device::init() {}
void Device::tick() {}