#include <PN532_I2C.h>
#include <NfcAdapter.h>

PN532_I2C pn532_i2c(Wire);

class NFCDevice: public czujnik {
  public:
    String action = "";
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
            } else {
                this->message = "blad zapisu";
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
        if (this->action == "nfc read" && !this->reading) {
            this->action = "";
            this->read();
        }

        if (this->action == "nfc write" && !this->reading) {
            this->action = "";
            this->write();
        }
    }

    void writeString(String field, String value) {
        if (field == "action") {
            this->action = value;
        }
        
        if (field == "content") {
            this->content = value;
        }

        if (field == "message") {
            this->message = value;
        }
    }

    String readString(String field) {
        if (field == "content") {
            return this->content;
        }

        if (field == "message") {
            return this->message;
        }
    }

    NFCDevice() {
        this->name = "nfc";
    }
};