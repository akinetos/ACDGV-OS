class GD: public Device {
  public :
    DFRobot_GR10_30 device = DFRobot_GR10_30(0x73, &Wire);
    boolean changed = false;
    String gesture = "";
    
    void init() {
      this->device.begin();
      this->device.enGestures(GESTURE_UP|GESTURE_DOWN|GESTURE_LEFT|GESTURE_RIGHT|GESTURE_CLOCKWISE_C|GESTURE_COUNTERCLOCKWISE_C);
    }
  
    void tick() {
        this->changed = false;
      if (this->device.getDataReady()) {
        uint16_t gestures = this->device.getGesturesState();
        if (gestures&GESTURE_UP) {
          this->gesture = "gora";
        }
        if (gestures&GESTURE_DOWN) {
          this->gesture = "dol";
        }
        if(gestures&GESTURE_LEFT) {
          this->gesture = "lewo";
        }
        if(gestures&GESTURE_RIGHT) {
          this->gesture = "prawo";
        }
        if(gestures&GESTURE_CLOCKWISE_C) {
          this->gesture = "CW";
        }
        if(gestures&GESTURE_COUNTERCLOCKWISE_C) {
          this->gesture = "CCW";
        }
        this->changed = true;
      }
    }

  GD() {}
};