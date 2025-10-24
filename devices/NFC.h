class NFCDevice: public Device {
  public:
    String content = "www.acdgv.dev";
    String message = "";
    bool reading = false;
    bool writting = false;
    NfcAdapter device = NfcAdapter(pn532_i2c);

    void read() {
        this->reading = true;
        if (this->device.tagPresent()) {
            NfcTag tag = this->device.read();
            NdefMessage message = tag.getNdefMessage();
            NdefRecord record = message.getRecord(0);
            int payloadLength = record.getPayloadLength();
            byte payload[payloadLength];
            record.getPayload(payload);
            this->message = "";
            for (int c = 1; c < payloadLength; c++) {
                this->message += (char)payload[c];
            }
        } else {
            this->message = "no tag";
        }
        this->reading = false;
    }

    void write() {
        this->writting = true;
        if (this->device.tagPresent()) {
            NdefMessage message = NdefMessage();
            message.addUriRecord(this->content);
            bool success = this->device.write(message);
            if (success) {
                this->message = "zapisano";
            }
        } else {
            this->message = "no tag";
        }
        this->writting = false;
    }

    void init() {
        this->device.begin();
    }

    void tick() {
        if (action == "nfc read" && !this->reading) {
            action = "";
            delay(1000);
            this->read();
        }

        if (action == "nfc write" && !this->reading) {
            action = "";
            delay(1000);
            this->write();
        }
    }

    NFCDevice() {}
};