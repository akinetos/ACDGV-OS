class czujnik {
  public:
    int address;
    boolean connected;
    boolean changed;
    char buttonPressed = NULL;
    String name;
    int orientationX = 1;

    czujnik();
    
    void virtual inicjuj(void);

    void virtual aktualizuj(void);

    double virtual czytajLiczbe(String);

    String virtual readString(String);

    void virtual writeString(String, String);

    boolean orientationChanged;
    String orientation;

    int position = 0;

    boolean shortPress = false;
    boolean longPress = false;
};

czujnik::czujnik() {}

void czujnik::inicjuj() {}
void czujnik::aktualizuj() {}
double czujnik::czytajLiczbe(String) {}
String czujnik::readString(String) {}
void czujnik::writeString(String, String) {}