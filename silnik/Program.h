class program {
  public:
    program() {}
    boolean initialised = false;
    boolean active = false;
    int activatedTimestamp = 0;
    int option = 0;
    String name = "";
    int menuLevel = -1;

    void virtual inicjuj(void) {
      this->initialised = true;
    }

    void virtual aktualizuj(void) {}

    void virtual setOption(int option) {
      this->option = option;
    }

    void virtual activate() {
      this->active = true;
      this->activatedTimestamp = millis();
    }

    void virtual update() {}
    
    void virtual rysuj() {}
};