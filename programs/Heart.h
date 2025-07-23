class Heart:public Program {
    public:
        int surfaceIndex = -1;

        void init(int surfaceIndex) {
            this->surfaceIndex = surfaceIndex;
        }

        void tick() {
            Surface & surface = surfaces[this->surfaceIndex];
            if (!surface.screensDisabled && hrs.changed) {
                channels[surface.channel].ports[0].screen.heading = (String)hrs.rate;
                channels[surface.channel].ports[0].screen.clear();
                channels[surface.channel].ports[0].screen.printHeading();
                channels[surface.channel].ports[0].screen.needsRefresh = true;

                channels[surface.channel].ports[1].screen.heading = (String)hrs.temperature;
                channels[surface.channel].ports[1].screen.clear();
                channels[surface.channel].ports[1].screen.printHeading();
                channels[surface.channel].ports[1].screen.needsRefresh = true;
            }
        }

        void becameActive() {}
    
    Heart() {}
};
