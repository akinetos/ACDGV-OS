#include <FS.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>

const int programsCount = 1;

#include "./engine/I2C.h";
I2C i2c = I2C();

#include "./engine/Program.h";
Program * programs[programsCount];

#include "./engine/Device.h";
#include "./devices/WiFi.h";
Wifi wifi = Wifi();

#include "./programs/Router.h";


String IpAddress2String(const IPAddress& ip) {
  char buffer[20];
  sprintf(buffer, "%i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
  return buffer;
}

void executeAll(String commandsString) {
  StaticJsonBuffer<2000> jsonBuffer;
  JsonArray& commands = jsonBuffer.parseArray(commandsString);
  for (int i=0; i<commands.size(); i++) {

  }
}


//------ SERVER ------
AsyncWebServer server(80);

void serverInit() {
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.serveStatic("/", SPIFFS, "/");
  server.begin();
}


//------ WEBSOCKET ------
WebSocketsServer webSocket = WebSocketsServer(81);
int wsOutgoingCount = 0;
int wsIncommingCount = 0;
int wsIncommingHandledCount = 0;
int wsIncommingIgnoredCount = 0;
String wsClientIp = "";
uint8_t wsClientNumber = 0;
boolean wsChanged = false;
boolean wsConnected = false;
String wsCommands = "";

void wsCommandsAdd(String command) {
  if (wsCommands != "") {
    wsCommands += ",";
  }
  wsCommands += command;
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED: {
      wsConnected = false;
      wsChanged = true;
    } break;
        
    case WStype_CONNECTED: {
      wsIncommingCount++;
      wsChanged = true;
      wsConnected = true;
      wsClientNumber = num;
      wsClientIp = IpAddress2String(webSocket.remoteIP(num));
      String command = "{\"a\":[[11,1,4,2,1,2]],\"v\":[" + (String)millis() + "]}";
      wsCommandsAdd(command);
    } break;
    
    case WStype_TEXT: {
      String commands = (char*)payload;
      executeAll(commands);
      wsIncommingCount++;
      wsChanged = true;
    } break;
  }
}

void websocketInit() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void websocketTick(String mode) {
  if (mode == "incomming") {
    webSocket.loop();
  }
  if (mode == "outgoing") {
    if (wsConnected && wsCommands != "") {
      webSocket.sendTXT(wsClientNumber, "[" + wsCommands + "]");
      wsOutgoingCount++;
      wsCommands = "";
    }
  }
}


void setup() {
  Serial.begin(115200);
  SPIFFS.begin();
  i2c.init();
  programs[0] = new Router();
  programs[0]->init();
  programs[0]->active = true;
  serverInit();
  websocketInit();
}


void loop() {
  websocketTick("incomming");
  
  for (int i=0; i<programsCount; i++) {
    if (programs[i]->active) {
      programs[i]->tick();
    }
  }

  websocketTick("outgoing");
}
