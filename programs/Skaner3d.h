class Skaner3d:public Program {
  public:
    void init() {
		this->initialised = true;
	}

	void tick() {
		for (int i=0; i<8; i++) {
			channels[0].ports[i].screen.needsRefresh = true;
		}
	}
	
    void draw() {
		double distance0 = channels[0].ports[0].devices[0]->readNumber("distance");
		double distance1 = channels[0].ports[1].devices[0]->readNumber("distance");
		double distance2 = channels[0].ports[2].devices[0]->readNumber("distance");
		double distance3 = channels[0].ports[3].devices[0]->readNumber("distance");
		double distance4 = channels[0].ports[4].devices[0]->readNumber("distance");
		double distance5 = channels[0].ports[5].devices[0]->readNumber("distance");
		double distance6 = channels[0].ports[6].devices[0]->readNumber("distance");
		double distance7 = channels[0].ports[7].devices[0]->readNumber("distance");

		for (int x=0; x<128; x+=5) {
			if (x < distance0) {
				channels[0].ports[0].screen.ssd1306.drawLine(x,0,x,31,SSD1306_WHITE);
			}
			if (x < distance1) {
				channels[0].ports[1].screen.ssd1306.drawLine(x,0,x,31,SSD1306_WHITE);
			}
			if (x < distance2) {
				channels[0].ports[2].screen.ssd1306.drawLine(x,0,x,31,SSD1306_WHITE);
			}
			if (x < distance3) {
				channels[0].ports[3].screen.ssd1306.drawLine(x,0,x,31,SSD1306_WHITE);
			}
			if (x < distance4) {
				channels[0].ports[4].screen.ssd1306.drawLine(x,0,x,31,SSD1306_WHITE);
			}
			if (x < distance5) {
				channels[0].ports[5].screen.ssd1306.drawLine(x,0,x,31,SSD1306_WHITE);
			}
			if (x < distance6) {
				channels[0].ports[6].screen.ssd1306.drawLine(x,0,x,31,SSD1306_WHITE);
			}
			if (x < distance7) {
				channels[0].ports[7].screen.ssd1306.drawLine(x,0,x,31,SSD1306_WHITE);
			}
		}
    }

  Skaner3d() {
	this->name = "skaner-3d";
  }
};
