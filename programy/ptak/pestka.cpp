class pestka {
    public:
        int timestamp;
        int port;
        int previousPort;
        int x;
        int y;
        int vectorX;
        int vectorY;
    
    pestka() {}

    pestka(int timestamp, int port, int x, int y) {
        this->timestamp = timestamp;
        this->port = port;
        this->previousPort = -1;
        this->x = x;
        this->y = y;
    }
};