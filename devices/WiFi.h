class Wifi: public Device {
  public:
    const char * networkName = "[[18,1,2],1]";
    const char * networkPassword = "rozrusznik2g";
    boolean connected = false;
    int attempts = 0;
    const int maxAttempts = 5;
    const int delayTime = 1000;

    void init() {
      WiFi.mode(WIFI_STA);
      WiFi.begin(this->networkName, this->networkPassword);
      while (WiFi.status() != WL_CONNECTED) {
        delay(this->delayTime);
        this->attempts++;
      }
      this->connected = (WiFi.status() == WL_CONNECTED);
    }
  
    void tick() {}

  Wifi() {}
};