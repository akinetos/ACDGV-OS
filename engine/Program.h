class Program {
  public:
    Program() {}
    boolean initialised = false;
    boolean active = false;
    int activatedTimestamp = 0;
    int option = 0;
    int counter = 0;

    void virtual init(void) {}

    void virtual tick(void) {}

    void virtual setOption(int option) {
      this->option = option;
    }

    void virtual justActivated() {
      this->activatedTimestamp = millis();
      this->counter = 0;
    }

    void virtual update() {}
};