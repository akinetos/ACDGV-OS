class Gamepad: public Device {
  public:
    Adafruit_seesaw device;
    int buttonApressedTime = 0;
    int time = 0;
    int connectionAttempts = 0;
  
    void init() {
      this->connected = this->device.begin(this->address);
      while (!this->connected && this->connectionAttempts < 10) {
        delay(100);
        this->connected = this->device.begin(this->address);
        this->connectionAttempts++;
      }
    }
  
    void tick() {
      if (this->connected) {
        this->time = millis();
        this->x = (512.0 - this->device.analogRead(14)) / 512.0;
        this->y = (512.0 - this->device.analogRead(15)) / 512.0;

        if (this->device.analogRead(5) == 0) {
          if (this->buttonApressedTime == 0) {
            this->buttonApressedTime = this->time;
          }
        }

        if (this->device.analogRead(5) > 0) {
          if (this->buttonApressedTime > 0) {
            if (this->time - this->buttonApressedTime > 1000) {
              this->longPress = true;
              this->shortPress = false;
            } else {
              this->shortPress = true;
              this->longPress = false;
            }
            this->buttonApressedTime = 0;
          } else {
            this->shortPress = false;
            this->longPress = false;
            this->buttonApressedTime = 0;
          }
        }
      }
    }

  Gamepad(int address) {
    this->address = address;
  }
};