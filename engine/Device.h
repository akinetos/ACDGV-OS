class Device {
  public:
    Device();
    void virtual init(void);
    void virtual tick(void);
};

Device::Device() {}

void Device::init() {}
void Device::tick() {}