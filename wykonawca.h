#include "czujniki/nachylenie.h";
#include "czujniki/dzojstik.h";
#include "czujniki/klawiatura-numeryczna.h";
#include "czujniki/nfc.h";
#include "czujniki/pamiec.h";
#include "czujniki/odleglosc.h";

#include "programy/ptak.h";
#include "programy/grawitacja.h";
#include "programy/ww.h";
#include "programy/logo.h";
#include "programy/kontrolery.h";
#include "programy/odleglosciomierz.h";
#include "programy/bateria.h";
#include "programy/kontakty.h";

class wykonawca {
    public:
        int wykonaj(JsonArray & komenda) {
            int output = 0;
            String commandType = komenda[0];

            if (commandType == "uruchom") {
                int programIndex = -1;
                String programName = komenda[1];
                boolean hasOption = komenda.size() == 3;
                
                for (int i=0; i<=indeksOstatniegoProgramu; i++)
                    if (programy[i]->name == programName)
                        programIndex = i;

                if (programIndex == -1) {
                    indeksOstatniegoProgramu++;
                    if (programName == "ptak") {
                        programy[indeksOstatniegoProgramu] = new Batterfly();
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

                output = programIndex;
            }

            return output;
        }

        int wykonaj(String komenda) {
            DynamicJsonBuffer dynamicJsonBuffer;
            JsonArray & json = dynamicJsonBuffer.parseArray(komenda);
            return this->wykonaj(json);
        }

        void skanuj() {
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
        }

        wykonawca() {}
};
