class Router:public Program {
    public:
      String networks[20];
      int networksCount = 0;
      int counter = 0;
      String mode = "";

      String IpAddress2String(const IPAddress& ip) {
        char buffer[20];
        sprintf(buffer, "%i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
        return buffer;
      }

      void init() {
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
        /*
        Surface & s2x1 = surfaces[0];
        RE & re = channels[0].ports[5].re;
        OLED & screen = channels[s2x1.channel].ports[1].screen;

        if (s2x1.facingUp) {
          if (this->mode == "status") {
            screen.heading = "stat";
            for (int i=0; i<this->networksCount; i++) {
              screen.lines[i] = this->networks[i];
            }
            screen.optionsCount = this->networksCount;
            screen.hasOptions = true;
            screen.needsRefresh = true;
          }
          
          if (this->mode == "networks") {
            screen.heading = "netw";
            for (int i=0; i<this->networksCount; i++) {
              screen.lines[i] = this->networks[i];
            }
            screen.optionsCount = this->networksCount;
            screen.hasOptions = true;
            screen.needsRefresh = true;
          }

          if (this->mode == "networks" && re.button) {
            this->mode = "network";
            screen.heading = screen.lines[screen.lineHovered];
            screen.hasOptions = false;
          }

          if (this->mode == "network") {
            screen.printHeading(2);
          }
        }
        
        Surface & s8x1 = surfaces[1];
        if (s8x1.facingUp) {
          OLED & topScreen = channels[s8x1.channel].ports[0].screen;
          topScreen.heading = "WiFi";
          topScreen.printHeading(2);
          for (int n=0; n<networksCount; n++) {
            OLED & screenN = channels[s8x1.channel].ports[n+1].screen;
            screenN.heading = this->networks[n];
            screenN.hasOptions = false;
            screenN.printHeading(1);
          }
        }
        */
      }

      void setOption(int option) {
        if (option == -1) {
          this->mode = "";
        }
        if (option == 0) {
          this->mode = "status";
        }
        if (option == 1) {
          this->mode = "networks";
        }
      }

      void becameActive() {}

    Router() {}
};
