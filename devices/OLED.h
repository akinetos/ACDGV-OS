class OLED {
  public:
    Adafruit_SH1106G sh1106 = Adafruit_SH1106G(128, 64, &Wire, -1);
    Adafruit_SSD1306 ssd1306 = Adafruit_SSD1306(128, 64, &Wire, -1);
    int width = 128;
    int height = 32;
    boolean connected = false;
    boolean needsRefresh = false;
    boolean needsInit = false;
    String type;
    boolean showLines = true;
    
    String lines[21];
    int lineScrollWidth[21];
    int lineScrollWait[21];
    int lineScrollTimestamp[21];
    int lineScrollStep[21];
    
    int lineHovered;
    int previousLineHovered;
    int lineHoveredTimestamp;
    
    int lineSelected = -1;
    
    String heading;
    boolean hasOptions;

    int brightness = 1;
    int currentBrightness = 2;
    
    int background = 1;
    int currentBackground = 1;
    int nextBackground = 0;
    
    int textSize = 1;
    int currentTextSize = 1;

    String textColor = "white";
    String currentTextColor = "white";
    
    int textLeft = 0;
    int textTop = 0;
    
    int rotation = 0;
    int currentRotation = 1;

    int offsetX = 0;
    int offsetY = 0;

    String state = "on";
    String currentState = "on";

    boolean scrollbarHovered = false;
    boolean backButtonHovered = false;

    boolean showRambugGuns = false;
    boolean showRambugFire = false;

    int rambugFireOriginX = 0;
    int rambugFireOriginY = 0;
    int rambugFireRadius = 10;
    String rambugRotation = "";
    String rambugFireDirection = "";

    int maxLineLength = 11;
    int normalWaitTime = 100;
    int lastWaitTime = 2000;

    int pointerRadius = 3;
    boolean pointerPressed = false;
    String pointerType = "circles";
    int pointerTimeLeft = 0;
    int pointerTimeRight = 0;

    boolean verticalScroll = false;
    int optionsCount = 0;
    int minOffsetY = 0;

    int textScroll = 0;
    int breadcrumbHovered = -1;
    
    void init(int width, int height, String type) {
      this->width = width;
      this->height = height;
      this->type = type;
      if (this->type == "sh1106") {
        this->sh1106 = Adafruit_SH1106G(this->width, this->height, &Wire, -1);
        if (this->sh1106.begin()) {
          this->setRotation();
          this->sh1106.setTextColor(SH110X_WHITE);
          this->sh1106.setTextSize(1);
          this->connected = true;
        }
      }
      if (this->type == "ssd1306") {
        this->ssd1306 = Adafruit_SSD1306(this->width, this->height, &Wire, -1);
        if (this->ssd1306.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
          this->setRotation();
          this->ssd1306.setTextColor(SSD1306_WHITE);
          this->ssd1306.setTextSize(1);
          this->background = 1;
          this->connected = true;
        }
      }
      if (this->connected) {
        this->needsRefresh = true;
      }
      this->needsInit = false;
    }

    boolean textSizeChanged() {
      return this->textSize != this->currentTextSize;
    }

    boolean brightnessChanged() {
      return this->brightness != this->currentBrightness;
    }

    boolean backgroundChanged() {
      return this->background != this->currentBackground;
    }

    boolean stateChanged() {
      return this->state != this->currentState;
    }

    void rambugGuns() {
      if (this->showRambugGuns) {
        this->ssd1306.fillCircle(0, 16, 10, SSD1306_WHITE);
        this->ssd1306.fillCircle(128, 16, 10, SSD1306_WHITE);
      }
    }

    void rambugFire() {
      if (this->showRambugFire) {
        this->rambugFireRadius += 1;
        if (this->rambugFireRadius < 40) {
          if (this->rambugFireDirection == "prawo") {
            this->rambugFireOriginX = 0;
            this->rambugFireOriginY = 16;
          }
          if (this->rambugFireDirection == "lewo") {
            this->rambugFireOriginX = 128;
            this->rambugFireOriginY = 16;
          }
          if (this->rambugFireDirection == "lewo" || this->rambugFireDirection == "prawo") {
            this->ssd1306.drawCircle(this->rambugFireOriginX, this->rambugFireOriginY, this->rambugFireRadius, SSD1306_WHITE);
          }
        } else {
          this->rambugFireDirection = "";
          this->rambugFireRadius = 10;
          this->showRambugFire = false;
        }
      }
    }

    void setState() {
      if (this->type == "ssd1306") {
        if (this->state == "off") {
          this->ssd1306.ssd1306_command(SSD1306_DISPLAYOFF);
        }
        if (this->state == "on") {
          this->ssd1306.ssd1306_command(SSD1306_DISPLAYON);
        }
      }
      this->currentState = this->state;
    }

    void tick() {
      if (this->stateChanged()) {
        this->setState();
      }
      if (this->state == "on") {
        if (this->backgroundChanged()) {
          this->setBackground();
        }
        if (this->nextBackground > 0) {
            this->background = this->nextBackground;
            this->nextBackground = 0;
        }
        if (this->brightnessChanged()) {
          this->setBrightness();
        }
        if (this->textSizeChanged()) {
          this->setTextSize();
        }
        if (this->rotationChanged()) {
          this->setRotation();
        }
      }
      this->refresh();
    }

    void setTextColor() {
      if (this->type == "ssd1306") {
        if (this->textColor == "white") {
          this->ssd1306.setTextColor(SSD1306_WHITE);
        }
        if (this->textColor == "black") {
          this->ssd1306.setTextColor(SSD1306_BLACK);
        }
      }
    }

    void printSelectedLine() {
      this->textSize = 4;
      this->setTextSize();
      this->textColor = "white";
      this->setTextColor();

      if (this->lineSelected > -1) {
        int scrollWidth = (this->lines[this->lineSelected].length() - 5) * 24;
        if (this->lineScrollWidth[this->lineSelected] <= 0) {
          this->lineScrollWidth[this->lineSelected] = 0;
          this->lineScrollWait[this->lineSelected] = this->lastWaitTime;
          this->lineScrollStep[this->lineSelected] = 3;
        } else if (this->lineScrollWidth[this->lineSelected] >= scrollWidth) {
          this->lineScrollWidth[this->lineSelected] = scrollWidth;
          this->lineScrollWait[this->lineSelected] = this->lastWaitTime;
          this->lineScrollStep[this->lineSelected] = -20;
        } else {
          this->lineScrollWait[this->lineSelected] = 0;
        }
        if ((millis() - this->lineScrollTimestamp[this->lineSelected]) > this->lineScrollWait[this->lineSelected]) {
          this->lineScrollWidth[this->lineSelected] += this->lineScrollStep[this->lineSelected];
          this->lineScrollTimestamp[this->lineSelected] = millis();
        }
      }
      
      if (this->type == "ssd1306") {
        if (this->lineSelected > -1) {
          this->ssd1306.setCursor(-1 * this->lineScrollWidth[this->lineSelected], 2);
          this->ssd1306.print(this->lines[this->lineSelected]);
        } else {
          this->ssd1306.setCursor(0, 0);
          this->ssd1306.print("");
        }
      }
    }

    void printHeading() {
      this->printHeading(2);
    }

    void printHeading(int size) {
      this->textSize = size;
      this->setTextSize();
      if (this->type == "ssd1306") {
        this->ssd1306.setCursor(0, 0);
        if (this->hasOptions) {
          this->ssd1306.fillRect(0, 0, (int)(this->width * 0.38), this->height, SSD1306_BLACK);
        } else {
          this->ssd1306.fillRect(0, 0, this->width, this->height, SSD1306_BLACK);
        }
        this->ssd1306.setTextColor(SSD1306_WHITE);
        this->ssd1306.print(this->heading);
      }
      if (this->type == "sh1106") {
        this->sh1106.setCursor(0, 0);
        if (this->hasOptions) {
          this->sh1106.fillRect(0, 0, (int)(this->width * 0.38), this->height, SH110X_BLACK);
        } else {
          this->sh1106.fillRect(0, 0, this->width, this->height, SH110X_BLACK);
        }
        this->sh1106.setTextColor(SH110X_WHITE);
        this->sh1106.print(this->heading);
      }
      this->needsRefresh = true;
    }

    void drawBackButton() {
      if (this->type == "ssd1306") {
        int buttonWidth = 10;
        this->ssd1306.fillRect(0, 0, buttonWidth, this->height, SSD1306_WHITE);
        this->ssd1306.setTextColor(SSD1306_BLACK);
        this->ssd1306.setCursor(2, this->height/2 - 4);
        this->ssd1306.setTextSize(1);
        this->ssd1306.print("<");
      }
      this->needsRefresh = true;
    }

    void printText(String message) {
      const int length = message.length();
      this->textSize = 2;
      this->setTextSize();
      if (this->type == "ssd1306") {
        this->ssd1306.setCursor(this->textScroll, 0);
        this->ssd1306.setTextColor(SSD1306_WHITE);
        this->ssd1306.setTextWrap(false);
        this->ssd1306.print(message);
      }
      this->textScroll--;
      if (this->textScroll < -100) {
        this->textScroll = 0;
      }
      this->needsRefresh = true;
    }

    void drawBreadcrumbs(String path, int cursorX) {
      const int length = path.length();
      int buttonWidth = 10;
      int count = 0;
      int segments[10] = {0,0,0,0,0,0,0,0,0,0};
      int segmentStart = 0;
      int textScrollLimit = this->width - length * 12;
      for (int i=0; i<length; i++) {
        if (path[i] == '/') {
          segments[count] = i - segmentStart;
          count++;
          segmentStart = i + 1;
        }
        if (i == (length - 1)) {
          segments[count] = i - segmentStart + 1;
        }
      }
      this->textSize = 2;
      this->setTextSize();
      
      if (this->type == "ssd1306") {
        this->ssd1306.setCursor(buttonWidth + 2 + this->textScroll, 9);
        this->ssd1306.setTextColor(SSD1306_WHITE);
        this->ssd1306.setTextWrap(false);
        this->ssd1306.print(path);
        
        segmentStart = 0;
        boolean breadcrumbIsHovered = false;
        for (int i=0; i<10; i++) {
          if (segments[i] != 0) {
            const int segmentLength = segments[i] * 12;
            if (cursorX > -1 && cursorX > textScroll + segmentStart + 12 && cursorX < textScroll + segmentStart + segmentLength + 12) {
              this->breadcrumbHovered = i;
              breadcrumbIsHovered = true;
              this->ssd1306.drawRect(textScroll + segmentStart + 12, 0, segmentLength, 32, SSD1306_WHITE);
            }
            segmentStart += segmentLength + 12;
          }
        }
        if (!breadcrumbIsHovered) {
          this->breadcrumbHovered = -1;
        }
      }

      if (length * 12 > this->width) {
        this->textScroll--;
        if (this->textScroll < textScrollLimit) {
          this->textScroll = textScrollLimit;
        }
      } else {
        this->textScroll = 0;
      }
      
      this->needsRefresh = true;
    }

    void drawScrollbar() {
      if (this->hasOptions) {
        if (this->type == "sh1106") {
          float start = this->offsetY/10 * (-1);
          float percentage = start / this->optionsCount;
          int length = (float)(this->height/10) / this->optionsCount * this->height;
          this->sh1106.fillRect(this->width - 10, 0, 10, this->height, SH110X_WHITE);
          this->sh1106.drawRect(this->width - 10, 0, 10, this->height, SH110X_WHITE);
          this->sh1106.fillRect(this->width - 10 + 2, (int)(percentage * height) + 2, 6, length, SH110X_WHITE);
        }
        if (this->type == "ssd1306") {
          float start = this->offsetY/10 * (-1);
          float percentage = start / this->optionsCount;
          int length = (float)(this->height/10) / this->optionsCount * this->height;
          this->ssd1306.fillRect(this->width - 10, 0, 10, this->height, SSD1306_BLACK);
          this->ssd1306.drawRect(this->width - 10, 0, 10, this->height, SSD1306_WHITE);
          this->ssd1306.fillRect(this->width - 10 + 2, (int)(percentage * height) + 2, 6, length, SSD1306_WHITE);
        }
      }
    }

    void drawRectangle(int x, int y) {
      this->ssd1306.fillRect(x, y, 128, 2, SSD1306_WHITE);
      this->needsRefresh = true;
    }

    void drawPoint(int x, int y) {
      this->ssd1306.drawPixel(x, y, SSD1306_WHITE);
      this->needsRefresh = true;
    }

    void setTextSize() {
      if (this->type == "sh1106") {
        this->sh1106.setTextSize(this->textSize);
      }
      if (this->type == "ssd1306") {
        this->ssd1306.setTextSize(this->textSize);
      }
      this->currentTextSize = this->textSize;
    }

    boolean rotationChanged() {
      return this->rotation != this->currentRotation;
    }

    void setRotation() {
      if (this->type == "sh1106") {
        this->sh1106.setRotation(this->rotation);
      }
      if (this->type == "ssd1306") {
        this->ssd1306.setRotation(this->rotation);
      }
      this->currentRotation = this->rotation;
    }

    boolean hasLongLines() {
      boolean output = false;
      for (int l=0; l<this->optionsCount; l++) {
        if (this->lines[l].length() > this->maxLineLength) {
          int scrollWidth = (this->lines[l].length() - this->maxLineLength) * 7;
          if ((millis() - this->lineScrollTimestamp[l]) > this->lineScrollWait[l]) {
            output = true;
          }
        }
      }
      return output;
    }

    boolean selectedLineIsLong() {
      return this->lines[this->lineSelected].length() > 5;
    }

    void printLines() {
      if (this->hasOptions) {
        this->textSize = 1;
        this->setTextSize();

        for (int l=0; l<this->optionsCount; l++) {
          if (this->lines[l].length() > this->maxLineLength) {
            int scrollWidth = (this->lines[l].length() - this->maxLineLength) * 7;
            if ((millis() - this->lineScrollTimestamp[l]) > this->lineScrollWait[l]) {
              if (this->lineScrollWidth[l] < scrollWidth) {
                this->lineScrollWidth[l]++;
                if (this->lineScrollWidth[l] == scrollWidth) {
                  this->lineScrollWait[l] = this->lastWaitTime;
                } else {
                  this->lineScrollWait[l] = this->normalWaitTime;
                }
              } else {
                this->lineScrollWidth[l] = 0;
                this->lineScrollWait[l] = this->lastWaitTime;
              }
              this->lineScrollTimestamp[l] = millis();
            }
          }
        }

        if (this->type == "sh1106") {
          for (int l=0; l<this->optionsCount; l++) {
            int y = l*10 + this->offsetY;
            this->sh1106.setCursor((int)(this->width*0.38) + 2 - this->lineScrollWidth[l], y + 2);
            if (this->showLines) {
              this->sh1106.drawRect((int)(this->width*0.38), y, (int)(this->width*0.62) - 10, 11, SH110X_WHITE);
            }
            if (l == this->lineSelected) {
              this->sh1106.fillRect((int)(this->width*0.38), y, (int)(this->width*0.62) - 10, 11, SH110X_WHITE);
              this->sh1106.setTextColor(SH110X_BLACK);
            } else {
              this->sh1106.setTextColor(SH110X_WHITE);
            }
            this->sh1106.print(this->lines[l]);
          }
        }
        
        if (this->type == "ssd1306") {
          this->ssd1306.fillRect(11, 0, 106, this->height, SSD1306_BLACK);
          for (int l=0; l<this->optionsCount; l++) {
            int y = l*10 + this->offsetY;
            this->ssd1306.setCursor(11 + 2 - this->lineScrollWidth[l], y + 2);
            if (this->showLines) {
              this->ssd1306.drawRect(11, y, 106, 11, SSD1306_WHITE);
            }
            if (l == this->lineSelected) {
              this->ssd1306.fillRect(11, y, 106, 11, SSD1306_WHITE);
              this->ssd1306.setTextColor(SSD1306_BLACK);
            } else {
              this->ssd1306.setTextColor(SSD1306_WHITE);
            }
            this->ssd1306.print(this->lines[l]);
          }
        }

        this->needsRefresh = true;
      }
    }

    void refresh() {
      if (this->type == "sh1106") {
        this->sh1106.display();
      }
      if (this->type == "ssd1306") {
        this->ssd1306.display();
      }
      this->needsRefresh = false;
    }

    void setBackground() {
      if (this->type == "sh1106") {
        if (this->background == 1) {
          this->sh1106.invertDisplay(false);
        } else {
          this->sh1106.invertDisplay(true);
        }
      }
      if (this->type == "ssd1306") {
        if (this->background == 1) {
          this->ssd1306.invertDisplay(false);
        } else {
          this->ssd1306.invertDisplay(true);
        }
      }
      this->currentBackground = this->background;
    }

    void setBrightness() {
      if (this->type == "sh1106") {
        if (this->brightness == 1) {
          this->sh1106.setContrast(0);
        }
        if (this->brightness == 2) {
          this->sh1106.setContrast(255);
        }
      }
      if (this->type == "ssd1306") {
        if (this->brightness == 1) {
          this->ssd1306.dim(true);
        }
        if (this->brightness == 2) {
          this->ssd1306.dim(false);
        }
      }
      this->currentBrightness = this->brightness;
    }
    
    void clear() {
      if (this->type == "sh1106") {
        this->sh1106.clearDisplay();
      }
      if (this->type == "ssd1306") {
        this->ssd1306.clearDisplay();
      }
      this->needsRefresh = true;
    }

    boolean isWhite(int16_t x, int16_t y) {
      return this->ssd1306.getPixel(x, y);
    }

    void drawPointer(int x, int y, String pointerType) {
      if (this->type == "ssd1306") {
        if (pointerType == "circle") {
          this->ssd1306.drawCircle(x, y, this->pointerRadius + 1, SSD1306_BLACK);
          this->ssd1306.drawCircle(x, y, this->pointerRadius, SSD1306_WHITE);
        }
      }
    }

    boolean lineHoveredChanged() {
      return this->lineHovered != this->previousLineHovered;
    }

    void blink() {
      this->background = 2;
      this->nextBackground = 1;
      this->needsRefresh = true;
    }

    void populate (JsonArray & json) {
      String branchName = json[0];
      if (json[1]) {
        const int amount = json[1].size();
        this->heading = branchName;
        this->hasOptions = amount > 0;
        this->optionsCount = amount;
        this->minOffsetY = -(this->optionsCount * 10) + this->height - 1;
        for (int i = 0; i < amount; i++) {
          String optionName = json[1][i][0];
          this->lines[i] = optionName;
        }
        this->lineSelected = -1;
        for (int i=0; i<=20; i++) {
          this->lineScrollWidth[i] = 0;
        }
        this->offsetY = 0;
        this->needsRefresh = true;
      } else {
        this->hasOptions = false;
        this->optionsCount = 0;
      }
    }

    void updateScrollbar (double tilt) {
      int tiltY = (int)(tilt * 20);
      this->offsetY -= tiltY;
      if (this->offsetY > 0) {
        this->offsetY = 0;
      }
      if (this->offsetY < this->minOffsetY) {
        this->offsetY = this->minOffsetY;
      }
    }

    void resetLineHovered() {
      this->lineHovered = -1;
      this->needsRefresh = true;
    }
    
  OLED(String type) { //sh1106 or ssd1306
    this->type = type;
    for (int l=0; l<this->optionsCount; l++) {
      this->lines[l] = "";
      this->lineHovered = -1;
      this->lineSelected = -1;
    }
  }
};
