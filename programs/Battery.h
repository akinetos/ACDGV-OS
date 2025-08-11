class Battery:public Program {
    public:
        int surfaceIndex = -1;

        void init() {
            this->surfaceIndex = 1;
            Surface & surface = surfaces[this->surfaceIndex];
            for (int i=0; i<=7; i++) {
                channels[surface.channel].ports[i].screen.heading = "";
                channels[surface.channel].ports[i].screen.clear();
                channels[surface.channel].ports[i].screen.printHeading();
                channels[surface.channel].ports[i].screen.needsRefresh = true;
            }
            this->initialised = true;
        }

        void tick() {
            Surface & surface = surfaces[this->surfaceIndex];
            if (gv.changed) {
                if (surface.facingUp) {
                    channels[surface.channel].ports[0].screen.heading = (String)gv.percentage;
                    channels[surface.channel].ports[0].screen.clear();
                    channels[surface.channel].ports[0].screen.printHeading();
                    channels[surface.channel].ports[0].screen.needsRefresh = true;

                    channels[surface.channel].ports[1].screen.heading = (String)gv.voltage;
                    channels[surface.channel].ports[1].screen.clear();
                    channels[surface.channel].ports[1].screen.printHeading();
                    channels[surface.channel].ports[1].screen.needsRefresh = true;

                    channels[surface.channel].ports[2].screen.heading = gv.status;
                    channels[surface.channel].ports[2].screen.clear();
                    channels[surface.channel].ports[2].screen.printHeading();
                    channels[surface.channel].ports[2].screen.needsRefresh = true;
                } else {
                    channels[0].ports[1].screen.heading = "BAT.";
                    channels[0].ports[1].screen.lines[0] = (String)gv.voltage;
                    channels[0].ports[1].screen.lines[1] = (String)gv.percentage;
                    channels[0].ports[1].screen.lines[2] = gv.status;
                    channels[0].ports[1].screen.hasOptions = true;
                    channels[0].ports[1].screen.needsRefresh = true;
                }
            }
        }
    
    Battery() {}
};
