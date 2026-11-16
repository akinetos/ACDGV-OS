class instrument: public program {
  	public:
		double odleglosci[8];

		void inicjuj() {
			this->initialised = true;
		}

		void aktualizuj() {
			if (this->active) {
				int aktywnyPort = -1;

				for (int i=0; i<8; i++) {
					port & konkretnyPort = i2c.kanaly[0].porty[i];
					double odleglosc = konkretnyPort.czujniki[0]->czytajLiczbe("odleglosc");
					if (odleglosc != 20 && odleglosc != this->odleglosci[i]) {
						this->odleglosci[i] = odleglosc;
						konkretnyPort.screen.needsRefresh = true;
						if (odleglosc < 100) {
							aktywnyPort = i;
							glosnik1.zagrajNute(aktywnyPort);
						}
					}
				}

				if (aktywnyPort == -1 && (millis() - glosnik1.poprzedniCzas) > 100)
					glosnik1.wycisz();
			}
		}
		
		void rysuj() {
			if (this->active) {
				for (int i=0; i<8; i++) {
					port & konkretnyPort = i2c.kanaly[0].porty[i];
					if (konkretnyPort.screen.needsRefresh) {
						double odleglosc = this->odleglosci[i];
						if (odleglosc < 20) odleglosc = 20;
						if (odleglosc > 128) odleglosc = 128;
						for (int x=0; x<odleglosc; x+=2)
							konkretnyPort.screen.ssd1306.drawLine(x, 0, x, 31, SSD1306_WHITE);
						for (int x=odleglosc-5; x<odleglosc; x++)
							konkretnyPort.screen.ssd1306.drawLine(x, 0, x, 31, SSD1306_WHITE);
						konkretnyPort.screen.printText((String)odleglosc);
					}
				}
			}
		}

		instrument() {
			this->name = "instrument";
		}
};
