class NFCProgram:public Program {
    public:
      void update() {
        channels[0].ports[7].screen.needsRefresh = true;
      }

      void init() {
        this->initialised = true;
      }

      void tick() {
        channels[0].ports[7].screen.printText(nfcDevice.message);
      }

      void setOption(int option) {
        if (option == 1) {
          action = "nfc read";
        }
        if (option == 2) {
          nfcDevice.content = "ERAZM";
          action = "nfc write";
        }
        if (option == 3) {
          nfcDevice.content = "FELIKS";
          action = "nfc write";
        }
      }

    NFCProgram() {}
};
