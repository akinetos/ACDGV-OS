class Skaner3d:public Program {
  	public:
		double distances[8];	

		void init() {
			this->initialised = true;
		}

		void tick() {
			for (int i=0; i<8; i++)
				channels[0].ports[i].screen.needsRefresh = true;
		}
		
		void draw() {
			for (int i=0; i<8; i++)
				this->distances[i] = channels[0].ports[i].devices[0]->readNumber("distance");

			
			for (int i=0; i<8; i++)
				for (int x=0; x<distances[i]; x++)
					channels[0].ports[i].screen.ssd1306.drawLine(x,0,x,31,SSD1306_WHITE);
		}

		Skaner3d() {
			this->name = "skaner-3d";
		}
};
