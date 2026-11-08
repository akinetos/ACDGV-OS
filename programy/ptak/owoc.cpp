class owoc {
    public:
        int port;
        int x;
        int y;
        int originalX;
        int originalY;
        int r = 7;
        boolean active;
    
    owoc() {
        this->x = random(8,120);
        this->y = random(2,30);
        this->r = random(6,12);
        this->port = random(1,8);
        this->originalX = this->x;
        this->originalY = this->y;
        this->active = true;
    }
};
