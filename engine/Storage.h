void initStorage() {
  SPIFFS.begin();
}

JsonArray & loadFromFile (String filePath) {
  File file = SPIFFS.open(filePath, "r");
  if (file) {
    String source = file.readString();
    int sourceLength = source.length() + 1; 
    char charArray[sourceLength];
    source.toCharArray(charArray, sourceLength);
    StaticJsonBuffer<2250> jsonBuffer;
    JsonArray & data = jsonBuffer.parseArray(charArray);
    return data;
  }
}