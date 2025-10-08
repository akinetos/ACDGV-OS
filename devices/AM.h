class AM: public Device {
  public:
    double previousX;
    double previousY;
    double previousZ;

    double x;
    double y;
    double z;

    int address;
    boolean changed = false;

    String orientation = "";
    String previousOrientation = "";
    boolean orientationChanged = false;

    void init() {
      i2c.writeTo(this->address, 0x2D, 0);      
      i2c.writeTo(this->address, 0x2D, 16);
      i2c.writeTo(this->address, 0x2D, 8);
      this->connected = true;
    }
    
    void tick() {
      if (this->connected) {
        this->changed = false;
        int regAddress = 0x32;
        byte buff[6];
        i2c.readFrom(this->address, regAddress, 6, buff);
        int x = (((int)buff[1]) << 8) | buff[0];   
        int y = (((int)buff[3]) << 8) | buff[2];
        int z = (((int)buff[5]) << 8) | buff[4];
      
        if (x > 65000) {
          x -= 65536;
        }
        if (y > 65000) {
          y -= 65536;
        }
      
        this->previousX = this->x;
        this->previousY = this->y;
        this->previousZ = this->z;
        
        this->x = ((x / 255.0) + this->previousX) / 2;
        this->y = ((y / 255.0) + this->previousY) / 2;
        this->z = z;
  
        if (this->x != this->previousX || this->y != this->previousY || this->z != this->previousZ) {
          this->changed = true;
          this->orientation = this->z < 65000 ? "up" : "down";
          this->orientationChanged = this->orientation != this->previousOrientation;
          if (this->orientationChanged) {
            this->previousOrientation = this->orientation;
          }
        }
      }
    }

  AM() {}

  AM(int address) {
    //0x1D (29), 0x53 (83)
    this->address = address;
  }
};
