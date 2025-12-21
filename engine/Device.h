class Device {
  public:
    int address;
    boolean connected;
    boolean changed;

    Device();
    
    void virtual init(void);

    void virtual tick(void);

    float axisX;
    float axisY;

    boolean orientationChanged;
    String orientation;

    double x;
    double y;
    double z;

    int position = 0;

    boolean shortPress = false;
    boolean longPress = false;
};

Device::Device() {}

void Device::init() {}
void Device::tick() {}