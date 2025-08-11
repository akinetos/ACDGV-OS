class Device {
  public:
    boolean connected;
    boolean changed;
    Device();
    void virtual init(void);
    void virtual tick(void);

    double x;
    double y;
    int position = 0;
};

Device::Device() {}

void Device::init() {}
void Device::tick() {}