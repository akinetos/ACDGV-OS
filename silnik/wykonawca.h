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
                        programy[indeksOstatniegoProgramu] = new Kontrolery();
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

        wykonawca() {}
};
