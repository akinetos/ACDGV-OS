class wykonawca {
    public:
        int execute(JsonArray & command) {
            int output = 0;
            String commandType = command[0];

            if (commandType == "run") {
                int programIndex = -1;
                String programName = command[1];
                boolean hasOption = command.size() == 3;
                
                for (int i=0; i<=lastProgramIndex; i++)
                    if (programy[i]->name == programName)
                        programIndex = i;

                if (programIndex == -1) {
                    lastProgramIndex++;
                    if (programName == "batterfly") {
                        programy[lastProgramIndex] = new Batterfly();
                    }
                    if (programName == "vv") {
                        programy[lastProgramIndex] = new VV();
                    }
                    if (programName == "logo") {
                        programy[lastProgramIndex] = new Logo();
                    }
                    if (programName == "skaner-3d") {
                        programy[lastProgramIndex] = new Skaner3d();
                    }
                    if (programName == "contacts") {
                        programy[lastProgramIndex] = new Contacts();
                    }
                    if (programName == "Kontrolery") {
                        programy[lastProgramIndex] = new Kontrolery();
                    }
                    if (programName == "gravity") {
                        programy[lastProgramIndex] = new Gravity();
                    }
                    if (programName == "battery") {
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
                    int optionValue = command[2];
                    programy[programIndex]->setOption(optionValue);
                }

                output = programIndex;
            }

            return output;
        }

        int execute(String commandString) {
            DynamicJsonBuffer dynamicJsonBuffer;
            JsonArray & command = dynamicJsonBuffer.parseArray(commandString);
            return this->execute(command);
        }

        wykonawca() {}
};
