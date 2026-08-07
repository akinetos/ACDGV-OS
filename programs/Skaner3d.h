class Skaner3d:public Program {
  	public:
		double distances[8];

		void init() {
			this->initialised = true;
		}

		void tick() {
			if (this->active) {
				for (int i=0; i<8; i++) {
					Port * port = & channels[0].ports[i];
					double distance = port->devices[0]->readNumber("distance");
					if (distance != 20 && distance != this->distances[i]) {
						this->distances[i] = distance;
						port->screen.needsRefresh = true;
					}
				}
			}
		}
		
		void draw() {
			if (this->active) {
				for (int i=0; i<8; i++) {
					OLED * screen = & channels[0].ports[i].screen;
					if (screen->needsRefresh) {
						double distance = this->distances[i];
						for (int x=0; x<distance; x+=2)
							screen->ssd1306.drawLine(x, 0, x, 31, SSD1306_WHITE);
						for (int x=distance-5; x<distance; x++)
							screen->ssd1306.drawLine(x, 0, x, 31, SSD1306_WHITE);
					}
				}
			}
		}

		Skaner3d() {
			this->name = "skaner-3d";
		}
};
