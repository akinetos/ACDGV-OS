class Program {
  public:
    Program();
    String mode;
    void virtual init(void);
    void virtual tick(void);
    void virtual setOption(int);
    void virtual becameActive();
};

Program::Program() {}

void Program::init() {}
void Program::tick() {}
void Program::setOption(int) {}
void Program::becameActive() {}