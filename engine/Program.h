class Program {
  public:
    Program() {}
    boolean initialised = false;
    boolean active = false;
    int activatedTimestamp = 0;
    int option = 0;
    String name = "";

    void virtual init(void) {
      this->initialised = true;
    }

    void virtual tick(void) {}

    void virtual setOption(int option) {
      this->option = option;
    }

    void virtual activate() {
      this->active = true;
      this->activatedTimestamp = millis();
    }

    void virtual update() {}
    
    void virtual draw() {}
};