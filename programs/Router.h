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
      boolean notificationSent = false;

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
          Serial.println("networksCount = " + (String)this->networksCount);
          for (int n = 0; n < this->networksCount; n++) {
            String name = wifiNetworks[n]["name"];
            String password = wifiNetworks[n]["password"];
            this->networks[n] = Network(name, password);
          }
        }
        this->initialised = true;
      }

      void tick() {
        if (wifi.connected) {
          if (!this->notificationSent) {
            byte data = 1;
            Wire.beginTransmission(0x10);
            Wire.write(data);
            Wire.endTransmission();
            this->notificationSent = true;
          }
        } else {
          Serial.println("not connected");
          String networkName = "[[18,1,2],1]";
          boolean found = false;
          int index = -1;

          for (int i=0; !found && i<this->networksCount; i++) {
            if (this->networks[i].name == networkName) {
              index = i;
              found = true;
            }
          }

          if (found) {
            Serial.println(networkName + " found");
            Network * network = &this->networks[index];
            wifi.connect(network->name, network->password);
            if (wifi.connected) {
              Serial.println("connected to " + networkName);
              Serial.println(wifi.ip);
            } else {
              Serial.println("still not connected");
            }
          } else {
            Serial.println(networkName + " not found");
          }
        }
      }

    Router() {}
};
