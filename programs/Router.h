class Router:public Program {
    public:
      String networks[20];
      int networksCount = 0;
      int counter = 0;
      int option = -1;
      int startedTime;

      void init() {
        this->startedTime = millis();
        String fullDirectory = "/wifi.json";
        File wifiFile = SPIFFS.open(fullDirectory, "r");
        if (wifiFile) {
          String source = wifiFile.readString();
          int sourceLength = source.length() + 1; 
          char charArray[sourceLength];
          source.toCharArray(charArray, sourceLength);
          StaticJsonBuffer<2000> jsonBuffer;
          JsonArray& wifiNetworks = jsonBuffer.parseArray(charArray);
          this->networksCount = wifiNetworks.size();
          for (int n = 0; n < this->networksCount; n++) {
            String ssid = wifiNetworks[n]["ssid"];
            this->networks[n] = (String)(n+1) + ". " + ssid;
          }
        }
      }

      void tick() {
        int menuLevel = interface.getMenuLevel();
        if (menuLevel == 3) {
          OLED & screen1 = channels[0].ports[1].screen;
          String heading = interface.levels[3];
          screen1.clear();
          screen1.printText(heading);
        }
      }

      void setOption(int option) {
        this->option = option;
      }

      void becameActive() {}

    Router() {}
};
