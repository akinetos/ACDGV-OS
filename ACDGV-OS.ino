#include <FS.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <Adafruit_seesaw.h>
#include <PN532_I2C.h>
#include <NfcAdapter.h>

String action = "";

#include "./engine/I2C.h";
I2C i2c = I2C();

const int programsCount = 1;
const int devicesCount = 2;

#include "./engine/Program.h";
Program * programs[programsCount];

#include "./engine/Device.h";
Device * devices[devicesCount];

#include "./devices/WiFi.h";
Wifi wifi = Wifi();

#include "./devices/AM.h";
AM accelerometer = AM(0x1D);

#include "./devices/Gamepad.h";
Gamepad gamepad = Gamepad(0x51);

#include "./devices/NFC.h";
NFCDevice nfcDevice = NFCDevice();

#include "./programs/Networks.h";


String IpAddress2String(const IPAddress& ip) {
  char buffer[20];
  sprintf(buffer, "%i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
  return buffer;
}

void executeAll(String commandsString) {
  StaticJsonBuffer<2000> jsonBuffer;
  JsonArray& commands = jsonBuffer.parseArray(commandsString);
  for (int i=0; i<commands.size(); i++) {
    JsonObject& command = commands[i];
    JsonArray & address = command["a"][0];
    JsonArray & values = command["v"];

    if (address[3] == 2) {
      if (address[4] == 2) {
        if (address[5] == 1) {
          action = "nfc read";
        }
        if (address[5] == 2) {
          String content = values[0];
          nfcDevice.content = content;
          action = "nfc write";
        }
      }
    }

  }
}


//------ SERVER ------
AsyncWebServer server(80);

void serverInit() {
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  
  server.serveStatic("/", SPIFFS, "/");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("server.on /");
    request->send(SPIFFS, "/index.html", String(), false);
  });
  
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
      Serial.println(" WStype_CONNECTED ");
      wsIncommingCount++;
      wsChanged = true;
      wsConnected = true;
      wsClientNumber = num;
      wsClientIp = IpAddress2String(webSocket.remoteIP(num));
      String command = "{\"a\":[[11,1,4,2,1,1]],\"v\":[" + (String)millis() + "]}";
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

  programs[0] = new Networks();
  programs[0]->init();
  programs[0]->active = true;
  
  serverInit();

  websocketInit();

  i2c.init();

  devices[0] = &accelerometer;
  devices[1] = &gamepad;
  //devices[2] = &nfcDevice;

  for (int i=0; i<devicesCount; i++) {
    devices[i]->init();
  }
}


void loop() {
  websocketTick("incomming");
  
  for (int i=0; i<devicesCount; i++) {
    devices[i]->tick();
  }

  for (int i=0; i<programsCount; i++) {
    if (programs[i]->active) {
      programs[i]->tick();
    }
  }

  if (wsConnected) {
    if (nfcDevice.changed) {
      String command = "{\"a\":[[11,1,4,2,1,2]],\"v\":[\"" + nfcDevice.message + "\"]}";
      wsCommandsAdd(command);
      nfcDevice.message = "";
    }

    if (gamepad.changed) {
      String command = "{\"a\":[[11,1,4,2,1,3]],\"v\":[" + (String)gamepad.x + ", " + (String)gamepad.y + "]}";
      wsCommandsAdd(command);
    }

    if (accelerometer.changed) {
      String command = "{\"a\":[[11,1,4,2,1,4]],\"v\":[" + (String)accelerometer.x + ", " + (String)accelerometer.y + "]}";
      wsCommandsAdd(command);
    }
  }

  websocketTick("outgoing");
}
