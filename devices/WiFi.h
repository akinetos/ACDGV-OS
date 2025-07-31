class Wifi: public Device {
  public:
    const char * networkName = "[[18,1,2],1]";
    const char * networkPassword = "rozrusznik2g";
    boolean connected = false;
    int attempts = 0;
    const int maxAttempts = 20;
    const int delayTime = 1000;
    String ip = "";

    String IpAddress2String(const IPAddress& ip) {
      char buffer[20];
      sprintf(buffer, "%i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
      return buffer;
    }

    void init() {
      WiFi.mode(WIFI_STA);
      WiFi.begin(this->networkName, this->networkPassword);
      
      while (!this->connected && this->attempts < this->maxAttempts) {
        delay(this->delayTime);
        this->connected = WiFi.status() == WL_CONNECTED;
        this->attempts++;
      }
      
      if (this->connected) {
        this->ip = this->IpAddress2String(WiFi.localIP());
      }
    }

    void connect(String name, String password) {
      WiFi.mode(WIFI_STA);
      WiFi.begin(name, password);
      
      while (!this->connected && this->attempts < this->maxAttempts) {
        delay(this->delayTime);
        this->connected = WiFi.status() == WL_CONNECTED;
        this->attempts++;
      }
      
      if (this->connected) {
        this->ip = this->IpAddress2String(WiFi.localIP());
      }
    }
  
    void tick() {}

  Wifi() {}
};