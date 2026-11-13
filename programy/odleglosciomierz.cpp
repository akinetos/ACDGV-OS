class Skaner3d:public program {
  	public:
		double distances[8];

		void inicjuj() {
			this->initialised = true;
		}

		void aktualizuj() {
			if (this->active) {
				int aktywnyPort = -1;
				for (int i=0; i<8; i++) {
					port * p = & i2c.kanaly[0].porty[i];
					double distance = p->czujniki[0]->readNumber("distance");
					if (distance != 20 && distance != this->distances[i]) {
						this->distances[i] = distance;
						p->screen.needsRefresh = true;
						if (distance < 100) {
							aktywnyPort = i;
							glosnik1.zagrajNute(aktywnyPort);
						}
					}
				}
				if (aktywnyPort == -1 && (millis() - glosnik1.poprzedniCzas) > 100) {
					glosnik1.wycisz();
				}
			}
		}
		
		void rysuj() {
			if (this->active) {
				for (int i=0; i<8; i++) {
					if (i2c.kanaly[0].porty[i].screen.needsRefresh) {
						double distance = this->distances[i];
						if (distance < 20) distance = 20;
						if (distance > 128) distance = 128;
						for (int x=0; x<distance; x+=2) {
							i2c.kanaly[0].porty[i].screen.ssd1306.drawLine(x, 0, x, 31, SSD1306_WHITE);
						}
						for (int x=distance-5; x<distance; x++) {
							i2c.kanaly[0].porty[i].screen.ssd1306.drawLine(x, 0, x, 31, SSD1306_WHITE);
						}
					}
				}
			}
		}

		Skaner3d() {
			this->name = "skaner-3d";
		}
};
