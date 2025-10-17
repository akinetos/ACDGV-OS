class NFCProgram:public Program {
    public:
      void update() {}

      void init() {
        this->initialised = true;
      }

      void tick() {}

      void setOption(int option) {
        if (option == 1) {
          action = "nfc read";
        }
        if (option == 2) {
          action = "nfc write";
        }
      }

    NFCProgram() {}
};
