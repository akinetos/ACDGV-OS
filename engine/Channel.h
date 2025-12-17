class Channel {
  public:
    int number;
    Port ports[8];

    void init(int number) {
      this->number = number;
      for (int p=0; p<8; p++) {
        this->ports[p].number = p;
        this->ports[p].channel = this->number;
        this->ports[p].init();
      }
      i2c.deactivate(this->number);
    }

    void tick() {
      for (int p = 0; p < 8; p++) {
        this->ports[p].tick();
      }
    }

    void display() {
      for (int p = 0; p < 8; p++) {
        this->ports[p].display();
      }
    }

    static int count(JsonArray & configSurfaces) {
      int channelsUsed[8];
      for (int i=0; i<8; i++) {
        channelsUsed[i] = -1;
      }
      for (int i=0; i<surfacesCount; i++) {
        int channel = configSurfaces[i]["channel"];
        boolean found = false;
        for (int j=0; j<8; j++) {
          if (channelsUsed[j] == channel) {
            found = true;
          }
        }
        if (!found) {
          int index = 0;
          for (int j=0; j<8; j++) {
            if (channelsUsed[j] >= 0) {
              index++;
            }
          }
          channelsUsed[index] = channel;
        }
      }
      int countChannels = 0;
      for (int i=0; i<8; i++) {
        if (channelsUsed[i] >= 0) {
          countChannels++;
        }
      }
      return countChannels;
    }
    
  Channel() {}
};
