class Wifi: public Device {
  public:
    boolean connected = false;
    int attempts = 0;
    const int maxAttempts = 10;
    const int delayTime = 1000;
    String ip = "";

    String IpAddress2String(const IPAddress& ip) {
      char buffer[20];
      sprintf(buffer, "%i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
      return buffer;
    }

    void init() {}

    void connect(String name, String password) {
      WiFi.mode(WIFI_STA);
      WiFi.begin(name, password);
      this->attempts = 0;
      
      while (!this->connected && this->attempts < this->maxAttempts) {
        delay(this->delayTime);
        this->connected = WiFi.status() == WL_CONNECTED;
        this->attempts++;
        Serial.println("attempt " + (String)this->attempts + "/" + (String)this->maxAttempts);
      }
      
      if (this->connected) {
        this->ip = this->IpAddress2String(WiFi.localIP());
      }
    }
  
    void tick() {}

  Wifi() {}
};