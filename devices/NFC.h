class NFCDevice: public Device {
  public:
    String nfcContent = "www.acdgv.dev";
    String nfcMessage = "";
    bool nfcReading = false;
    bool nfcWritting = false;
    PN532_I2C pn532_i2c(&Wire);
    NfcAdapter device = NfcAdapter(pn532_i2c);

    void read() {
        nfcReading = true;
        if (this->device.tagPresent()) {
            NfcTag tag = this->device.read();
            NdefMessage message = tag.getNdefMessage();
            NdefRecord record = message.getRecord(0);
            int payloadLength = record.getPayloadLength();
            byte payload[payloadLength];
            record.getPayload(payload);
            String text = "";
            for (int c = 1; c < payloadLength; c++) {
                text += (char)payload[c];
            }
            nfcMessage = text;
        } else {
            nfcMessage = "no tag";
        }
        nfcReading = false;
    }

    void write() {
        nfcWritting = true;
        if (this->device.tagPresent()) {
            NdefMessage message = NdefMessage();
            message.addUriRecord(nfcContent);
            bool success = this->device.write(message);
            if (success) {
                nfcMessage = "zapisano";
            }
        } else {
            nfcMessage = "no tag";
        }
        nfcWritting = false;
    }

    void init() {
        this->device.begin();
    }

    void tick() {
        if (action == "nfc read" && !nfcReading) {
            action = "";
            delay(1000);
            read();
        }

        if (action == "nfc write" && !nfcReading) {
            action = "";
            delay(1000);
            write();
        }
    }

    NFCDevice() {}
};