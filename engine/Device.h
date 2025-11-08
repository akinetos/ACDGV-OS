class Device {
  public:
    boolean connected;
    boolean changed;
    Device();
    void virtual init(void);
    void virtual tick(void);
    boolean virtual buttonApressed();

    double x;
    double y;
    float axisY;
    int position = 0;
};

Device::Device() {}

void Device::init() {}
void Device::tick() {}