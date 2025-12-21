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
        this->port = random(1,8);
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
        Star stars[STARS_COUNT];
        Pestka pestki[PESTKI_COUNT];

        void drawBatterfly() {
            int channel = -1;
            int port = -1;
            int x = 0;
            int y = 0;

            if (surfacesCount > 1) {
                Surface & s2x1 = surfaces[0];
                Surface & s8x1 = surfaces[1];

                if (s2x1.facingUp && s2x1.pointerPort == 1) {
                    channel = s2x1.channel;
                    port = s2x1.pointerPort;
                    x = s2x1.getRelativeX();
                    y = s2x1.getRelativeY();
                }

                if (s8x1.facingUp) {
                    channel = s8x1.channel;
                    port = s8x1.pointerPort;
                    x = s8x1.getRelativeX();
                    y = s8x1.getRelativeY();
                }
            } else {
                Surface & s8x1 = surfaces[0];
                channel = s8x1.channel;
                port = s8x1.pointerPort;
                x = s8x1.getRelativeX();
                y = s8x1.getRelativeY();
            }

            if (channel != -1 && port > 0) {
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
                if (this->scores == STARS_COUNT) {
                    this->over = true;
                    endedAt = millis();
                }
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
            for (int port=1; port<7; port++) {
                int charIndex = port - 1;
                String text = "";
                char character = message1.charAt(charIndex);
                for (int i=0; i<20; i++) {
                    text += character;
                }
                channels[0].ports[port].screen.printText(text);
            }
            channels[0].ports[7].screen.printText(message2);
        }

        void drawProgress() {
            OLED & screen = channels[0].ports[0].screen;
            screen.ssd1306.setCursor(64, 4);
            screen.ssd1306.setTextColor(SSD1306_WHITE);
            screen.ssd1306.setTextWrap(false);
            screen.ssd1306.print((String)this->scores + "/" + (String)STARS_COUNT);

            /*
            Port & p07 = channels[0].ports[7];
            if (this->scoresChanged || p07.devices[0]->changed || p07.devices[1]->changed) {
                p07.screen.clear();
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
                if (gamepad.buttonPressed()) {
                    p07.screen.sh1106.fillCircle(20, 50, 12, SH110X_WHITE);
                    p07.screen.sh1106.fillCircle(20 - (int)(p07.devices[0]->x * 20), 50 - (int)(p07.devices[0]->y * 20), radius, SH110X_BLACK);
                } else {
                    p07.screen.sh1106.drawCircle(20, 50, 12, SH110X_WHITE);
                    p07.screen.sh1106.fillCircle(20 - (int)(p07.devices[0]->x * 20), 50 - (int)(p07.devices[0]->y * 20), radius, SH110X_WHITE);
                }

                p07.screen.sh1106.drawRect(
                    64, 33, 
                    44, 31,
                    SH110X_WHITE
                );
                for (int i=0; i<p07.devices[1]->position; i++) {
                    p07.screen.sh1106.drawLine(
                        64 + i * 2 + 2, 32 + 3, 
                        64 + i * 2 + 2, 64 - 3,
                        SH110X_WHITE
                    );
                }

                p07.screen.needsRefresh = true;
            }
            */
        }

        void updateBatterfly() {
            Surface & surface = surfaces[this->surfaceIndex];
            this->vectorX = accelerometer.x * 10;
            this->vectorY = accelerometer.y * 10;
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
            Surface & surface = surfaces[this->surfaceIndex];
            
            int index = -1;
            if (gamepad.shortPress) {
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
            if (surfacesCount > 1) {
                Surface & s2x1 = surfaces[0];
                Surface & s8x1 = surfaces[1];

                if (s2x1.facingUp) {
                    for (int i=0; i<PESTKI_COUNT; i++) {
                        if (this->pestki[i].timestamp > 0) {
                            int t = (int)((millis() - this->pestki[i].timestamp) / 1000);
                            if (this->pestki[i].x >= 0 && this->pestki[i].x < 128 && this->pestki[i].y >= 0 && this->pestki[i].y < 64 && t <= 4) {
                                this->pestki[i].port = s2x1.drawCircle(this->pestki[i].x, this->pestki[i].y, (4 - t) + 2);
                            }
                        }    
                    }
                }
                
                if (s8x1.facingUp) {
                    for (int i=0; i<PESTKI_COUNT; i++) {
                        if (this->pestki[i].timestamp > 0) {
                            int t = (int)((millis() - this->pestki[i].timestamp) / 1000);
                            if (this->pestki[i].x >= 0 && this->pestki[i].x < 128 && this->pestki[i].y >= 0 && this->pestki[i].y < 256 && t <= 4) {
                                this->pestki[i].port = s8x1.drawCircle(this->pestki[i].x, this->pestki[i].y, (4 - t) + 2);
                            }
                        }    
                    }
                }
            } else {
                Surface & s8x1 = surfaces[0];
                for (int i=0; i<PESTKI_COUNT; i++) {
                    if (this->pestki[i].timestamp > 0) {
                        int t = (int)((millis() - this->pestki[i].timestamp) / 1000);
                        if (this->pestki[i].x >= 0 && this->pestki[i].x < 128 && this->pestki[i].y >= 0 && this->pestki[i].y < 256 && t <= 4) {
                            this->pestki[i].port = s8x1.drawCircle(this->pestki[i].x, this->pestki[i].y, (4 - t) + 2);
                        }
                    }    
                }
            }
        }

        void init() {
            this->surfaceIndex = 0;
            for (int i=0; i<STARS_COUNT; i++) {
                this->stars[i] = Star();
            }
            for (int i=0; i<PESTKI_COUNT; i++) {
                this->pestki[i] = Pestka();
            }
            this->initialised = true;
        }

        void tick() {
            Surface & surface = surfaces[this->surfaceIndex];
            
            this->counter++;

            if (this->counter == 1) {
                surface.refreshScreens();
            }
            
            if (this->over) {
                this->drawProgress();
                int time = (endedAt - this->activatedTimestamp) / 1000;
                String message1 = "BRAWO!";
                String message2 = "czas: " + String(time) + " s";
                this->splash(message1, message2);
            } else {
                this->updatePointer();
                this->updateBatterfly();
                this->updatePestki();
                surface.clear();
                this->drawStars();
                this->drawBatterfly();
                this->drawPestki();
                this->detectColisions();
                this->drawProgress();
            }
        }

        void update() {
            if (this->scoresChanged) {
                channels[0].ports[1].screen.needsRefresh = true;
            }
        }
    
    Batterfly() {}
};
