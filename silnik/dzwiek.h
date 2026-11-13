#define GLOSNIK_1 14

class dzwiek {
    public:
        int oktawa4[8];
        int nuta = 1000;
        int poprzedniCzas = 0;
        int indeksNuty = 0;
    
    void zagrajNute(int i) {
        tone(GLOSNIK_1, oktawa4[i]);
        this->poprzedniCzas = millis();
    }

    void nastepnaNuta() {
        this->indeksNuty++;
        if (this->indeksNuty >= 8)
            this->indeksNuty = 0;
    }

    void inicjuj() {
        pinMode(GLOSNIK_1, OUTPUT);
    }

    void aktualizuj() {
        if (millis() - this->poprzedniCzas >= this->nuta) {
            this->nastepnaNuta();
            this->zagrajNute(indeksNuty);
        }
    }

    void wycisz() {
        noTone(GLOSNIK_1);
    }
    
    dzwiek() {
        this->oktawa4[0] = 262; // C4
        this->oktawa4[1] = 294; // D4
        this->oktawa4[2] = 330; // E4
        this->oktawa4[3] = 349; // F4
        this->oktawa4[4] = 392; // G4
        this->oktawa4[5] = 440; // A4
        this->oktawa4[6] = 494; // B4
        this->oktawa4[7] = 523; // C4
    }
};
