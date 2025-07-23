class Gamepad: public Device {
  public:
    int address;
    Adafruit_seesaw device;
    float axisX;
    float axisY;
    int buttonApressedTime = 0;
    int time = 0;
  
    void init() {
      while (!this->device.begin(this->address)) {
        delay(100);
      }
    }
  
    void tick() {
      this->time = millis();
      this->axisX = (512.0 - this->device.analogRead(14)) / 512.0;
      this->axisY = (512.0 - this->device.analogRead(15)) / 512.0;

      if (this->device.analogRead(5) == 0) {
        if (this->buttonApressedTime == 0) {
          this->buttonApressedTime = this->time;
        }
      }

      if (this->device.analogRead(5) > 0) {
        if (this->buttonApressedTime > 0) {
          this->buttonApressedTime = 0;
        }
      }
    }

    boolean buttonApressed() {
      return this->buttonApressedTime == this->time;
    }

  Gamepad(int address) {
    this->address = address;
  }
};