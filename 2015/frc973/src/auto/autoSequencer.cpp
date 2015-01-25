#include "autoSequencer.hpp"

namespace frc973 {

AutoSequencer::AutoSequencer()
{
    // add a dummy command
    addSequential(new AutoCommand());

    commandsCompleted = 0;
    currOuterCommand = 0;
}

void AutoSequencer::addSequential(AutoCommand* command)
{
    addCommand(newCommand(command, "seq"));
}

void AutoSequencer::addConcurrent(AutoCommand* command)
{
    addCommand(newCommand(command, "con"));
}

void AutoSequencer::addCommand(AutoSequencer::COMMAND* command)
{
    if (command->type == "con" && sequence.back().back()->type == "con") {
        sequence.back().push_back(command);
    }
    else {
        sequence.push_back(std::vector<AutoSequencer::COMMAND*> (1, command));
    }
}

AutoSequencer::COMMAND* AutoSequencer::newCommand(AutoCommand* command, std::string type)
{
    COMMAND* c = new COMMAND;
    c->type = type;
    c->cmd = command;
    return c;
}

void AutoSequencer::init()
{
}

bool AutoSequencer::run()
{
    if (sequence.size() == 0)
    {
        printf("the current auto mode has no commands");
        return true;
    }

    if (currOuterCommand < sequence.size()) {
        for (unsigned int v=0; v < sequence[currOuterCommand].size(); v++) {
            if (sequence[currOuterCommand][v]->cmd->run()) {
                commandsCompleted += 1;
            }
        }

        if (commandsCompleted >= sequence[currOuterCommand].size()) {
            commandsCompleted = 0;
            currOuterCommand += 1;
        }
    }
    else {
        return true;
    }

    return false;
}

}
