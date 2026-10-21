#include <FS.h>

DynamicJsonBuffer jsonBuffer;

class katalog {
  public:
    static void init() {
      SPIFFS.begin();
    }

    static JsonArray & load (String filePath) {
      File file = SPIFFS.open(filePath, "r");
      if (file) {
        String source = file.readString();
        jsonBuffer.clear();
        JsonArray & data = jsonBuffer.parseArray(source.c_str());
        return data;
      }
    }
};