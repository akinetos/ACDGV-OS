class Executor {
    public:
        int execute(JsonArray & command) {
            int output = 0;
            String commandType = command[0];

            if (commandType == "run") {
                int programIndex = -1;
                String programName = command[1];
                boolean hasOption = command.size() == 3;
                
                for (int i=0; i<=lastProgramIndex; i++)
                    if (programs[i]->name == programName)
                        programIndex = i;

                if (programIndex == -1) {
                    lastProgramIndex++;
                    if (programName == "batterfly") {
                        programs[lastProgramIndex] = new Batterfly();
                    }
                    if (programName == "vv") {
                        programs[lastProgramIndex] = new VV();
                    }
                    if (programName == "logo") {
                        programs[lastProgramIndex] = new Logo();
                    }
                    if (programName == "skaner-3d") {
                        programs[lastProgramIndex] = new Skaner3d();
                    }
                    if (programName == "contacts") {
                        programs[lastProgramIndex] = new Contacts();
                    }
                    if (programName == "Kontrolery") {
                        programs[lastProgramIndex] = new Kontrolery();
                    }
                    if (programName == "gravity") {
                        programs[lastProgramIndex] = new Gravity();
                    }
                    if (programName == "battery") {
                        programs[lastProgramIndex] = new Battery();
                    }
                    programIndex = lastProgramIndex;
                }

                if (!programs[programIndex]->active) {
                    if (!programs[programIndex]->initialised)
                        programs[programIndex]->init();
                    programs[programIndex]->activate();
                    activeProgram = programIndex;
                }

                if (hasOption) {
                    int optionValue = command[2];
                    programs[programIndex]->setOption(optionValue);
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

        Executor() {}
};
