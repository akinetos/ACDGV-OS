#include "./Pixel.h";
#include "./OLED.h";
#include "./port.h";

class kanal {
  public:
    int number;
    port porty[8];

    static int count(JsonArray & configSurfaces) {
      int channelsUsed[8];
      for (int i=0; i<8; i++) {
        channelsUsed[i] = -1;
      }
      for (int i=0; i<configSurfaces.size(); i++) {
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

  kanal() {}
};
