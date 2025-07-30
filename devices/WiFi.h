class Wifi: public Device {
  public:
    const char * networkName = "[[18,1,2],1]";
    const char * networkPassword = "rozrusznik2g";
    boolean connected = false;
    int attempts = 0;
    const int maxAttempts = 20;
    const int delayTime = 1000;

    void init() {
      WiFi.mode(WIFI_STA);
      WiFi.begin(this->networkName, this->networkPassword);
      while (!this->connected && this->attempts < this->maxAttempts) {
        delay(this->delayTime);
        this->connected = WiFi.status() == WL_CONNECTED;
        this->attempts++;
      }
    }
  
    void tick() {}

  Wifi() {}
};