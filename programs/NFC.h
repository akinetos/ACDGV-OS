class NFCProgram:public Program {
    public:
      void update() {
        channels[0].ports[7].screen.needsRefresh = true;
      }

      void init() {
        this->initialised = true;
      }

      void tick() {
        if (this->active) {
          channels[0].ports[7].screen.printText(devices[2]->readString("message"));
        }
      }

      void setOption(int option) {
        if (option == 1) {
          action = "nfc read";
        }
        if (option == 2) {
          devices[2]->writeString("content", "ERAZM");
          action = "nfc write";
        }
        if (option == 3) {
          devices[2]->writeString("content", "FELIKS");
          action = "nfc write";
        }
      }

    NFCProgram() {
      this->name = "NFC";
    }
};
