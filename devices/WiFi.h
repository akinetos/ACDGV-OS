String wifiNetworkName = "";
String wifiNetworkPassword = "";
String wifiIpAddress = "";
String wifiMacAddress = "";
int wifiConnectAttempts = 0;
boolean wifiConnected = false;
int wifiMaxConnectAttempts = 5;

String IpAddress2String(const IPAddress& ip) {
  char buffer[20];
  sprintf(buffer, "%i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
  return buffer;
}

void wifiInit() {
  String fullDirectory = "/wifi.json";
  File wifiFile = SPIFFS.open(fullDirectory, "r");
  OLED & screen = channels[0].ports[1].screen;
  if (wifiFile) {
    screen.hasOptions = true;
    screen.lines[0] = "connecting";
    String source = wifiFile.readString();
    int sourceLength = source.length() + 1; 
    char charArray[sourceLength];
    source.toCharArray(charArray, sourceLength);
    StaticJsonBuffer<2000> jsonBuffer;
    JsonArray& wifiNetworks = jsonBuffer.parseArray(charArray);
    int networksCount = wifiNetworks.size();
    int reconnectDelay = 2000;
    for (int networkIndex = 0; networkIndex < networksCount; networkIndex++) {
      if (WiFi.status() != WL_CONNECTED) {
        wifiConnectAttempts = 0;
        String ssid = wifiNetworks[networkIndex]["ssid"];
        String password = wifiNetworks[networkIndex]["password"];
        wifiNetworkName = ssid;
        wifiNetworkPassword = password;
        screen.lines[1] = wifiNetworkName;
        WiFi.disconnect();
        WiFi.begin(wifiNetworkName, wifiNetworkPassword);
        while(WiFi.status() != WL_CONNECTED && wifiConnectAttempts < wifiMaxConnectAttempts) {
          wifiConnectAttempts++;
          screen.clear();
          screen.lines[2] = "N-" + (String)(networkIndex + 1) + "/" + (String)networksCount + " A-" + (String)wifiConnectAttempts + "/" + (String)wifiMaxConnectAttempts;
          screen.printLines();
          i2c.activate(0, 1);
          screen.refresh();
          delay(reconnectDelay);
        }
      }
    }

    if (WiFi.status() == WL_CONNECTED) {
      wifiIpAddress = IpAddress2String(WiFi.localIP());
      wifiMacAddress = WiFi.macAddress();
      screen.lines[0] = "connected";
      screen.lines[1] = wifiNetworkName;
      screen.lines[2] = wifiIpAddress;
      wifiConnected = true;
    } else {
      screen.lines[0] = "not connected";
    }
  }
}