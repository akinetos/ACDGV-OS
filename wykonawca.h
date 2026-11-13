#include "czujniki/nachylenie.cpp";
#include "czujniki/dzojstik.cpp";
#include "czujniki/klawiatura-numeryczna.cpp";
#include "czujniki/nfc.cpp";
#include "czujniki/pamiec.cpp";
#include "czujniki/odleglosc.cpp";

#include "programy/ptak/program.cpp";
#include "programy/grawitacja.cpp";
#include "programy/ww.cpp";
#include "programy/logo.cpp";
#include "programy/kontrolery.cpp";
#include "programy/odleglosciomierz.cpp";
#include "programy/bateria.cpp";
#include "programy/kontakty.cpp";

class wykonawca {
    public:
        int deviceIndexGamepad = -1;
        int uruchomProgram(JsonArray & komenda) {
            int programIndex = -1;
            String programName = komenda[1];
            boolean hasOption = komenda.size() == 3;
            
            for (int i=0; i<=indeksOstatniegoProgramu; i++)
                if (programy[i]->name == programName)
                    programIndex = i;

            if (programIndex == -1) {
                indeksOstatniegoProgramu++;
                if (programName == "ptak") {
                    programy[indeksOstatniegoProgramu] = new ptak();
                }
                if (programName == "ww") {
                    programy[indeksOstatniegoProgramu] = new VV();
                }
                if (programName == "logo") {
                    programy[indeksOstatniegoProgramu] = new Logo();
                }
                if (programName == "skaner-3d") {
                    programy[indeksOstatniegoProgramu] = new Skaner3d();
                }
                if (programName == "kontakty") {
                    programy[indeksOstatniegoProgramu] = new Contacts();
                }
                if (programName == "kontrolery") {
                    programy[indeksOstatniegoProgramu] = new kontrolery();
                }
                if (programName == "grawitacja") {
                    programy[indeksOstatniegoProgramu] = new Gravity();
                }
                if (programName == "bateria") {
                    programy[indeksOstatniegoProgramu] = new Battery();
                }
                programIndex = indeksOstatniegoProgramu;
            }

            if (!programy[programIndex]->active) {
                if (!programy[programIndex]->initialised)
                    programy[programIndex]->inicjuj();
                programy[programIndex]->activate();
                aktywnyProgram = programIndex;
            }

            if (hasOption) {
                int optionValue = komenda[2];
                programy[programIndex]->setOption(optionValue);
            }

            return programIndex;
        }

        int ustawieniaKontrolera(JsonArray & komenda) {
            this->deviceIndexGamepad = i2c.znajdzCzujnik("dzojstik");
            String wybrany = komenda[1];
            if (wybrany == "kablowy") {
                i2c.czujniki[this->deviceIndexGamepad]->orientationX = -1;
            }
            if (wybrany == "zintegrowany") {
                i2c.czujniki[this->deviceIndexGamepad]->orientationX = 1;
            }
            Serial.println("wybrany: " + wybrany);
        }

        int ustawieniaDzwieku(JsonArray & komenda) {
            String rodzaj = komenda[1];
            Serial.println("dzwiek: " + rodzaj);
        }

        void zamknijProgram() {
            if (aktywnyProgram != -1) {
                programy[aktywnyProgram]->active = false;
            }
            aktywnyProgram = -1;
            animacja.inicjuj(1);
        }

        void inicjujCzujniki() {
            Wire.beginTransmission(29);
            if (Wire.endTransmission() == 0) {
                i2c.indeksOstatniegoCzujnika++;
                i2c.czujniki[i2c.indeksOstatniegoCzujnika] = new AM(29);
            }

            Wire.beginTransmission(81);
            if (Wire.endTransmission() == 0) {
                i2c.indeksOstatniegoCzujnika++;
                i2c.czujniki[i2c.indeksOstatniegoCzujnika] = new Gamepad(81);
            }

            Wire.beginTransmission(36);
            if (Wire.endTransmission() == 0) {
                i2c.indeksOstatniegoCzujnika++;
                i2c.czujniki[i2c.indeksOstatniegoCzujnika] = new NFCDevice();
            }

            Wire.beginTransmission(75);
            if (Wire.endTransmission() == 0) {
                i2c.indeksOstatniegoCzujnika++;
                i2c.czujniki[i2c.indeksOstatniegoCzujnika] = new Keypad();
            }

            Wire.beginTransmission(80);
            if (Wire.endTransmission() == 0) {
                i2c.indeksOstatniegoCzujnika++;
                i2c.czujniki[i2c.indeksOstatniegoCzujnika] = new Memory();
            }

            for (int p=0; p<8; p++) {
                i2c.kanaly[0].porty[p].czujniki[0] = new DistanceSensor(0x29);
                i2c.kanaly[0].porty[p].czujniki[0]->inicjuj();
            }

            i2c.inicjujCzujniki();
        }

        int wykonaj(JsonArray & komenda) {
            int output = 0;
            String commandType = komenda[0];
            if (commandType == "uruchom") {
                output = this->uruchomProgram(komenda);
            }
            if (commandType == "kontroler") {
                output = this->ustawieniaKontrolera(komenda);
            }
            if (commandType == "dzwiek") {
                output = this->ustawieniaDzwieku(komenda);
            }
            return output;
        }

        int wykonaj(String komenda) {
            DynamicJsonBuffer dynamicJsonBuffer;
            JsonArray & json = dynamicJsonBuffer.parseArray(komenda);
            return this->wykonaj(json);
        }

        wykonawca() {}
};
