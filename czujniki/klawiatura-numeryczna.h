#include <SparkFun_Qwiic_Keypad_Arduino_Library.h>

class Keypad: public czujnik {
  public:
    KEYPAD device;
    int buttonPressedTime = 0;
  
    void inicjuj() {
      int status = this->device.begin();
      this->connected = status == 1;
    }
  
    void aktualizuj() {
      if (this->connected) {
        this->device.updateFIFO();

        this->buttonPressed = this->device.getButton();

        if  (this->buttonPressed != NULL) {
          this->shortPress = true;
        } else {
          this->shortPress = false;
          this->longPress = false;
        }
      }
    }

    boolean anyKeyPressed() {
      char button = this->device.getButton();
      return button != NULL;
    }

  Keypad() {
    this->name = "klawiatura-numeryczna";
  }
};