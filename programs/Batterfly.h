class Star {
    public:
        int port;
        int x;
        int y;
        int originalX;
        int originalY;
        int r = 7;
        boolean active;
    
    Star() {
        this->x = random(8,120);
        this->y = random(2,30);
        this->r = random(6,12);
        this->port = random(0,8);
        this->originalX = this->x;
        this->originalY = this->y;
        this->active = true;
    }
};

class Pestka {
    public:
        int timestamp;
        int port;
        int previousPort;
        int x;
        int y;
        int vectorX;
        int vectorY;
    
    Pestka() {}

    Pestka(int timestamp, int port, int x, int y) {
        this->timestamp = timestamp;
        this->port = port;
        this->previousPort = -1;
        this->x = x;
        this->y = y;
    }
};

const int STARS_COUNT = 124;
const int PESTKI_COUNT = 10;

int endedAt = 0;

class Batterfly:public Program {
    public:
        int wingsSpread = 10;
        int vectorX = 0;
        int vectorY = 0;
        int surfaceIndex = -1;
        int scores = 0;
        boolean scoresChanged = false;
        boolean over = false;
        int option = 0;
        Star stars[STARS_COUNT];
        Pestka pestki[PESTKI_COUNT];
        int startedTime;

        void clearScreens() {
            Surface & surface = surfaces[this->surfaceIndex];
            for (int p=0; p<8; p++) {
                OLED & screen = channels[surface.channel].ports[p].screen;
                if (screen.needsRefresh) {
                    screen.clear();
                }
            }
        }

        void drawBatterfly() {
            Surface & s2x1 = surfaces[0];
            Surface & s8x1 = surfaces[1];

            int channel = -1;
            int port = -1;
            int x = 0;
            int y = 0;

            if (s8x1.facingUp) {
                channel = s8x1.channel;
                port = s8x1.pointerPort;
                x = s8x1.getRelativeX();
                y = s8x1.getRelativeY();
            }
            
            if (s2x1.facingUp && s2x1.pointerPort == 1) {
                channel = s2x1.channel;
                port = s2x1.pointerPort;
                x = s2x1.getRelativeX();
                y = s2x1.getRelativeY();
            }

            if (channel != -1 && port != -1) {
                OLED & screen = channels[channel].ports[port].screen;
                if (screen.type == "ssd1306") {
                    if (this->option == 0) {
                        for (int i=1; i<=this->wingsSpread; i++) {
                            screen.ssd1306.drawCircle(x - i - 2, y, i, SSD1306_WHITE);
                            screen.ssd1306.drawCircle(x + i + 2, y, i, SSD1306_WHITE);
                        }
                        screen.ssd1306.drawCircle(x, y - 8, 2, SSD1306_WHITE);
                        screen.ssd1306.drawCircle(x, y - 5, 2, SSD1306_WHITE);
                        screen.ssd1306.drawCircle(x, y + 5, 2, SSD1306_WHITE);
                        screen.ssd1306.drawCircle(x, y + 8, 2, SSD1306_WHITE);
                    }
                    if (this->option == 1) {
                        for (int i=1; i<=this->wingsSpread; i++) {
                            screen.ssd1306.drawRect(x - 2 - i * 2, y - i, i * 2, i * 2, SSD1306_WHITE);
                            screen.ssd1306.drawRect(x + 2,         y - i, i * 2, i * 2, SSD1306_WHITE);
                        }
                        screen.ssd1306.drawRect(x - 2, y - 10, 4, 4, SSD1306_WHITE);
                        screen.ssd1306.drawRect(x - 2, y - 6, 4, 4, SSD1306_WHITE);
                        screen.ssd1306.drawRect(x - 2, y + 2, 4, 4, SSD1306_WHITE);
                        screen.ssd1306.drawRect(x - 2, y + 6, 4, 4, SSD1306_WHITE);
                    }
                    
                    this->wingsSpread--;
                    if (this->wingsSpread < 5) {
                        this->wingsSpread = 10;
                    }
                }
            }
        }

        void handleCollision(int port, int x, int y) {
            boolean found = false;
            Surface & surface = surfaces[this->surfaceIndex];
            for (int s=0; !found && s<STARS_COUNT; s++) {
                Star & star = this->stars[s];
                if (star.active && star.port == port) {
                    int distanceX = star.x - x;
                    int distanceY = star.y - y;
                    double distance = sqrt(distanceX * distanceX + distanceY * distanceY);
                    if (distance <= star.r) {
                        star.active = false;
                        found = true;
                    }
                }
            }
            if (found) {
                this->scores++;
                this->scoresChanged = true;
            } 
        }

        void detectColisions() {
            Surface & surface = surfaces[this->surfaceIndex];
            int port = surface.pointerPort;
            int x = surface.getRelativeX();
            int y = surface.getRelativeY();
            OLED & screen = channels[surface.channel].ports[port].screen;
            if (screen.isWhite(x, y)) {
                screen.blink();
                this->handleCollision(port, x, y);
            }
            for (int i=0; i<PESTKI_COUNT; i++) {
                port = this->pestki[i].port;
                x = this->pestki[i].x;
                y = this->pestki[i].y - (this->pestki[i].port * 32);
                OLED & screenPestka = channels[surface.channel].ports[port].screen;
                if (screenPestka.isWhite(x, y)) {
                    screenPestka.blink();
                    this->handleCollision(port, x, y);
                }
            }
        }

        void updateStars() {
            double diffX = accelerometer.x - accelerometer.previousX;
            double diffY = accelerometer.y - accelerometer.previousY;
            if (sqrt(diffX * diffX) > 0.01 || sqrt(diffY * diffY) > 0.01) {
                Surface & surface = surfaces[this->surfaceIndex];
                for (int s=0; s<STARS_COUNT; s++) {
                    Star & star = this->stars[s];
                    star.x = star.originalX + accelerometer.x * 100;
                    star.y = star.originalY + accelerometer.y * 100;
                    channels[surface.channel].ports[star.port].screen.needsRefresh = true;
                }
            }
        }

        void drawStars() {
            Surface & surface = surfaces[this->surfaceIndex];
            for (int s=0; s<STARS_COUNT; s++) {
                Star & star = this->stars[s];
                if (star.active) {
                    OLED & screen = channels[surface.channel].ports[star.port].screen;
                    screen.ssd1306.fillCircle(star.x, star.y, star.r-1, SSD1306_WHITE);
                    screen.ssd1306.drawCircle(star.x, star.y, star.r, SSD1306_BLACK);
                }
            }
        }

        void splash(String message1, String message2) {
            for (int p=0; p<6; p++) {
                OLED & screen = channels[this->surfaceIndex].ports[p].screen;
                screen.heading = message1.charAt(p);
                screen.printHeading();
                screen.needsRefresh = true;
            }
            OLED & screen = channels[this->surfaceIndex].ports[7].screen;
            screen.printText(message2);
            screen.needsRefresh = true;
        }

        void drawProgress() {
            Port & p07 = channels[0].ports[7];
            if (this->scoresChanged || p07.re.changed || p07.accelerometer.changed) {
                p07.screen.clear();

                if (this->scores == STARS_COUNT) {
                    this->over = true;
                    endedAt = millis();
                }
                p07.screen.sh1106.drawRect(
                    0, 0, 
                    128, 32,
                    SH110X_WHITE
                );
                for (int s=0; s<this->scores; s++) {
                    p07.screen.sh1106.drawLine(
                        s+2, 2, 
                        s+2, 29,
                        SH110X_WHITE
                    );
                }
                this->scoresChanged = false;

                int radius = 2;
                if (gamepad.buttonApressed()) {
                    p07.screen.sh1106.fillCircle(20, 50, 12, SH110X_WHITE);
                    p07.screen.sh1106.fillCircle(20 - (int)(p07.accelerometer.x * 20), 50 - (int)(p07.accelerometer.y * 20), radius, SH110X_BLACK);
                } else {
                    p07.screen.sh1106.drawCircle(20, 50, 12, SH110X_WHITE);
                    p07.screen.sh1106.fillCircle(20 - (int)(p07.accelerometer.x * 20), 50 - (int)(p07.accelerometer.y * 20), radius, SH110X_WHITE);
                }

                p07.screen.sh1106.drawRect(
                    64, 33, 
                    44, 31,
                    SH110X_WHITE
                );
                for (int i=0; i<p07.re.position; i++) {
                    p07.screen.sh1106.drawLine(
                        64 + i * 2 + 2, 32 + 3, 
                        64 + i * 2 + 2, 64 - 3,
                        SH110X_WHITE
                    );
                }

                p07.screen.needsRefresh = true;
            }
        }

        void updateBatterfly() {
            Surface & surface = surfaces[this->surfaceIndex];
            this->vectorX = -accelerometer.x * 10;
            this->vectorY = -accelerometer.y * 10;
            channels[surface.channel].ports[surface.pointerPort].screen.needsRefresh = true;
        }

        void updatePointer() {
            Surface & surface = surfaces[this->surfaceIndex];
            if (surface.pointerPortChanged()) {
                channels[surface.channel].ports[surface.pointerPreviousPort].screen.needsRefresh = true;
            }
            if (surface.pointerPositionY < 0) {
                surface.pointerPositionY = 0;
                surface.pointerPort = 0;
            }
        }

        void updatePestki() {
            Port & p07 = channels[0].ports[7];
            Surface & surface = surfaces[this->surfaceIndex];
            int index = -1;
            if (gamepad.buttonApressed()) {
                for (int i=0; i<PESTKI_COUNT; i++) {
                    if (this->pestki[i].timestamp == 0) {
                        index = i;
                    }
                }
                if (index > -1) {
                    this->pestki[index].timestamp = millis();
                    this->pestki[index].vectorX = this->vectorX;
                    this->pestki[index].vectorY = this->vectorY;
                    this->pestki[index].x = surface.pointerPositionX;
                    this->pestki[index].y = surface.pointerPositionY;
                    channels[surface.channel].ports[this->pestki[index].port].screen.needsRefresh = true;
                }
            }
            for (int i=0; i<PESTKI_COUNT; i++) {
                if (this->pestki[i].timestamp > 0) {
                    this->pestki[i].x -= this->pestki[i].vectorX;
                    this->pestki[i].y -= this->pestki[i].vectorY;
                    int t = (millis() - this->pestki[i].timestamp) / 1000;
                    if (!(this->pestki[i].x >= 0 && this->pestki[i].x < 128 && this->pestki[i].y >= 0 && this->pestki[i].y < 256 && t <= 4)) {
                        this->pestki[i].timestamp = 0;
                    }
                    channels[surface.channel].ports[this->pestki[i].port].screen.needsRefresh = true;
                }
            }
        }

        void drawPestki() {
            Surface & s2x1 = surfaces[0];
            Surface & s8x1 = surfaces[1];

            if (s8x1.facingUp) {
                for (int i=0; i<PESTKI_COUNT; i++) {
                    if (this->pestki[i].timestamp > 0) {
                        int t = (int)((millis() - this->pestki[i].timestamp) / 1000);
                        if (this->pestki[i].x >= 0 && this->pestki[i].x < 128 && this->pestki[i].y >= 0 && this->pestki[i].y < 256 && t <= 4) {
                            this->pestki[i].port = s8x1.fillCircle(this->pestki[i].x, this->pestki[i].y, (4 - t) + 2);
                        }
                    }    
                }
            }

            if (s2x1.facingUp) {
                for (int i=0; i<PESTKI_COUNT; i++) {
                    if (this->pestki[i].timestamp > 0) {
                        int t = (int)((millis() - this->pestki[i].timestamp) / 1000);
                        if (this->pestki[i].x >= 0 && this->pestki[i].x < 128 && this->pestki[i].y >= 0 && this->pestki[i].y < 64 && t <= 4) {
                            this->pestki[i].port = s2x1.fillCircle(this->pestki[i].x, this->pestki[i].y, (4 - t) + 2);
                        }
                    }    
                }
            }
        }

        void init() {
            this->startedTime = millis();
            this->surfaceIndex = 1;
            for (int i=0; i<STARS_COUNT; i++) {
                this->stars[i] = Star();
            }
            for (int i=0; i<PESTKI_COUNT; i++) {
                this->pestki[i] = Pestka();
            }
        }

        void tick() {
            Surface & s2x1 = surfaces[0];
            Surface & s8x1 = surfaces[1];

            if (s8x1.facingUp) {
                if (this->over) {
                    int time = (endedAt - this->startedTime) / 1000;
                    this->splash("BRAWO!", "czas: " + String(time) + " sekund");
                } else {
                    this->updatePointer();
                    //this->updateStars();
                    this->updateBatterfly();
                    this->updatePestki();
                    this->clearScreens();
                    this->drawStars();
                    this->drawBatterfly();
                    this->drawPestki();
                    this->detectColisions();
                    this->drawProgress();
                }
            }

            if (s2x1.facingUp) {
                this->updateBatterfly();
                this->updatePestki();
                s2x1.clear();
                this->drawBatterfly();
                this->drawPestki();
            }
        }

        void setOption(int option) {
            this->option = option;
        }

        void becameActive() {}
    
    Batterfly() {}
};
