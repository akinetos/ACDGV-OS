class czujnik {
  public:
    int address;
    boolean connected;
    boolean changed;
    char buttonPressed = NULL;
    String name;

    czujnik();
    
    void virtual init(void);

    void virtual tick(void);

    double virtual readNumber(String);

    String virtual readString(String);

    void virtual writeString(String, String);

    boolean orientationChanged;
    String orientation;

    int position = 0;

    boolean shortPress = false;
    boolean longPress = false;
};

czujnik::czujnik() {}

void czujnik::init() {}
void czujnik::tick() {}
double czujnik::readNumber(String) {}
String czujnik::readString(String) {}
void czujnik::writeString(String, String) {}