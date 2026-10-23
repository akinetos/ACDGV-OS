class wykonawca {
    public:
        int wykonaj(JsonArray & komenda) {
            int output = 0;
            String commandType = komenda[0];

            if (commandType == "uruchom") {
                int programIndex = -1;
                String programName = komenda[1];
                boolean hasOption = komenda.size() == 3;
                
                for (int i=0; i<=lastProgramIndex; i++)
                    if (programy[i]->name == programName)
                        programIndex = i;

                if (programIndex == -1) {
                    lastProgramIndex++;
                    if (programName == "ptak") {
                        programy[lastProgramIndex] = new Batterfly();
                    }
                    if (programName == "ww") {
                        programy[lastProgramIndex] = new VV();
                    }
                    if (programName == "logo") {
                        programy[lastProgramIndex] = new Logo();
                    }
                    if (programName == "skaner-3d") {
                        programy[lastProgramIndex] = new Skaner3d();
                    }
                    if (programName == "kontakty") {
                        programy[lastProgramIndex] = new Contacts();
                    }
                    if (programName == "kontrolery") {
                        programy[lastProgramIndex] = new Kontrolery();
                    }
                    if (programName == "grawitacja") {
                        programy[lastProgramIndex] = new Gravity();
                    }
                    if (programName == "bateria") {
                        programy[lastProgramIndex] = new Battery();
                    }
                    programIndex = lastProgramIndex;
                }

                if (!programy[programIndex]->active) {
                    if (!programy[programIndex]->initialised)
                        programy[programIndex]->init();
                    programy[programIndex]->activate();
                    activeProgram = programIndex;
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
