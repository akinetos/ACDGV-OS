class Network {
  public:
    String name;
    String password;
    Network() {}
    Network(String name, String password) {
      this->name = name;
      this->password = password;
    }
};

class Router:public Program {
    public:
      Network networks[20];
      int networksCount = 0;
      int counter = 0;

      void init() {
        String fullDirectory = "/config/wifi.json";
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
            String name = wifiNetworks[n]["name"];
            String password = wifiNetworks[n]["password"];
            this->networks[n] = Network(name, password);
          }
        }
        this->initialised = true;
      }

      void tick() {
        int pathLevel = interface.getPathLevel();
        if (pathLevel == 3) {
          String networkName = interface.segments[3];
          boolean found = false;
          for (int i=0; !found && i<this->networksCount; i++) {
            if (this->networks[i].name == networkName) {
              wifi.connect(this->networks[i].name, this->networks[i].password);
              found = true;
            }
          }
          OLED & screen1 = channels[0].ports[1].screen;
          screen1.clear();
          if (found) {
            screen1.printText("found");
          } else {
            screen1.printText("not found");
          }
        }
      }

    Router() {}
};
