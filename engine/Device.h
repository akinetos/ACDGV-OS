class Device {
  public:
    int address;
    boolean connected;
    boolean changed;
    char buttonPressed = NULL;

    Device();
    
    void virtual init(void);

    void virtual tick(void);

    double virtual readNumber(String);

    boolean orientationChanged;
    String orientation;

    int position = 0;

    boolean shortPress = false;
    boolean longPress = false;
};

Device::Device() {}

void Device::init() {}
void Device::tick() {}
double Device::readNumber(String) {}