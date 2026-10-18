//https://botland.com.pl/gravity-moduly-dzwiekowe/23332-gravity-modul-rozpoznawania-glosu-offline-i2c-uart-dfrobot-sen0539-en-6959420923342.html
//https://wiki.dfrobot.com/sen0539-en#target_0
//https://wiki.dfrobot.com/sen0539-en/docs/21331

class VRS: public Device {
  public :
    DFRobot_DF2301Q_I2C device = DFRobot_DF2301Q_I2C();
    boolean changed = false;
    String command = "";
    
    void init() {
      /*
      this->device.begin();
      this->device.setVolume(4);
      this->device.setMuteMode(0);
      this->device.setWakeTime(20);
      uint8_t wakeTime = 0;
      wakeTime = this->device.getWakeTime();
      */
    }
  
    void tick() {
      /*
      uint8_t CMDID = this->device.getCMDID();
      switch (CMDID) {
        case 103:                                                  //If the command is “Turn on the light”
          Serial.println("received 'Turn on the light', command flag '103'");  //Serial transmits "received"Turn on the light",command flag"103
          break;

        case 104:                                                  //If the command is “Turn off the light”
          Serial.println("received'Turn off the light',command flag'104'");  //The serial transmits "received"Turn off the light",command flag"104""
          break;

        default:
          if (CMDID != 0) {
            Serial.print("CMDID = ");  //Printing command ID
            Serial.println(CMDID);
          }
      }
      delay(300);
      */
    }

  VRS() {}
};