class NFCProgram:public Program {
    public:
      void update() {
        channels[0].ports[7].screen.needsRefresh = true;
      }

      void init() {
        this->initialised = true;
      }

      void tick() {
        channels[0].ports[7].screen.printText(nfcMessage);
      }

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
