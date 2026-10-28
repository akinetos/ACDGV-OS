class Skaner3d:public program {
  	public:
		double distances[8];

		void inicjuj() {
			this->initialised = true;
		}

		void aktualizuj() {
			if (this->active) {
				for (int i=0; i<8; i++) {
					port * p = & kanaly[0].porty[i];
					double distance = p->czujniki[0]->readNumber("distance");
					if (distance != 20 && distance != this->distances[i]) {
						this->distances[i] = distance;
						p->screen.needsRefresh = true;
					}
				}
			}
		}
		
		void rysuj() {
			if (this->active) {
				for (int i=0; i<8; i++) {
					if (kanaly[0].porty[i].screen.needsRefresh) {
						double distance = this->distances[i];
						if (distance < 20) distance = 20;
						if (distance > 128) distance = 128;
						for (int x=0; x<distance; x+=2) {
							kanaly[0].porty[i].screen.ssd1306.drawLine(x, 0, x, 31, SSD1306_WHITE);
						}
						for (int x=distance-5; x<distance; x++) {
							kanaly[0].porty[i].screen.ssd1306.drawLine(x, 0, x, 31, SSD1306_WHITE);
						}
					}
				}
			}
		}

		Skaner3d() {
			this->name = "skaner-3d";
		}
};
