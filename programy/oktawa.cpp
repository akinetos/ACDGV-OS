class oktawa: public program {
    public:
        int nuta = 1000;
        int poprzedniCzas = 0;
        int indeksNuty = 0;

    void nastepnaNuta() {
        this->indeksNuty++;
        if (this->indeksNuty >= 8)
            this->indeksNuty = 0;
    }

    void inicjuj() {}

    void aktualizuj() {
        if (this->active) {
            if (millis() - this->poprzedniCzas >= this->nuta) {
                this->nastepnaNuta();
                glosnik1.zagrajNute(indeksNuty);
                this->poprzedniCzas = millis();
            }
        }
    }
};
