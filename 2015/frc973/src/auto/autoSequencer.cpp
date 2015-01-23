#include "autoSequencer.hpp"

namespace frc973 {

AutoSequencer::AutoSequencer()
{
    // add a dummy command
    addSequential(new AutoCommand());

    currInnerCommand = 0;
    currOuterCommand = 0;
}

void AutoSequencer::addSequential(AutoCommand* command)
{
    addCommand(newCommand(command, "seq"));
}

void AutoSequencer::addConcurrent(AutoCommand* command)
{
    COMMAND* c = newCommand(command, "con");

    if (sequence.back().back()->type == "con")
    {
        addCommand(c, sequence.size());
    }
    else
    {
        addCommand(c);
    }
}

// order = -1 by default
void AutoSequencer::addCommand(AutoSequencer::COMMAND* command, int order)
{
    if (order != -1)
    {
        sequence[order].push_back(command);
    }
    else
    {
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

    printf("hit 1\n");
    if (currOuterCommand < sequence.size()) {
        printf("hit 2\n");
        if (currInnerCommand < sequence[currOuterCommand].size()) {

            if (sequence[currOuterCommand][currInnerCommand]->cmd->run()) {
                printf("hit 3\n");
                currInnerCommand += 1;

                if (currInnerCommand >= sequence[currOuterCommand].size()) {
                    currInnerCommand = 0;
                    currOuterCommand += 1;

                    if (currOuterCommand >= sequence.size()) {
                        return true;
                    }
                }
            }
        }
    }
    else {
        return true;
    }

    return false;
}

}
