class Program {
  public:
    Program() {}
    boolean active = false;
    int becameActiveTime = 0;
    int option = 0;
    void virtual init(void) {}
    void virtual tick(void) {}
    void setOption(int option) {
      this->option = option;
    }
    void virtual becameActive() {
      this->becameActiveTime = millis();
    }
};