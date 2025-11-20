class Gamepad: public Device {
  public:
    Adafruit_seesaw device;
    float axisX;
    float axisY;
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

    boolean buttonPressed() {
      return this->buttonApressedTime == this->time;
    }

  Gamepad(int address) {
    this->address = address;
  }
};