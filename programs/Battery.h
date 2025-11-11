class Battery:public Program {
    public:
        void tick() {
            if (gv.changed) {
                OLED & screen = channels[0].ports[1].screen;
                screen.lines[0] = (String)gv.voltage;
                screen.lines[1] = (String)gv.percentage;
                screen.lines[2] = gv.status;
                screen.hasOptions = true;
                screen.optionsCount = 3;
                screen.printLines();
            }
        }
    
    Battery() {}
};
