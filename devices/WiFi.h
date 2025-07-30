void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED: {} break;
        
    case WStype_CONNECTED: {} break;
    
    case WStype_TEXT: {} break;
  }
}

class Wifi: public Device {
  public:
    const char * networkName = "[[18,1,2],1]";
    const char * networkPassword = "rozrusznik2g";
    boolean connected = false;
    int attempts = 0;
    const int maxAttempts = 20;
    const int delayTime = 1000;

    WebSocketsServer webSocket = WebSocketsServer(81);
    uint8_t wsClientNumber = 0;
    String wsCommands = "";
    int wsOutgoingCount = 0;

    void init() {
      WiFi.mode(WIFI_STA);
      WiFi.begin(this->networkName, this->networkPassword);
      while (!this->connected && this->attempts < this->maxAttempts) {
        delay(this->delayTime);
        this->connected = WiFi.status() == WL_CONNECTED;
        this->attempts++;
      }

      if (this->connected) {
        this->webSocket.begin();
        this->webSocket.onEvent(webSocketEvent);
      }
    }
  
    void tick() {
      if (this->connected) {
        this->webSocket.loop();
        if (this->wsCommands != "") {
          this->webSocket.sendTXT(this->wsClientNumber, "[" + this->wsCommands + "]");
          this->wsOutgoingCount++;
          this->wsCommands = "";
        }
      }
    }

  Wifi() {}
};