#include "autoSequencer.hpp"


AutoSequencer::AutoSequencer()
{
    // add a dummy command
    addSequential(new AutoCommand());
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
    for (unsigned int n=0;n<sequence.front().size();n++)
    {
        sequence.front()[n]->cmd->init();
    }
}

bool AutoSequencer::run()
{
    if (sequence.size() == 0)
    {
        printf("the current auto mode has no commands");
        return true;
    }

    for (unsigned int k=0;k<sequence.size();k++)
    {
        for (unsigned int v=0;v<sequence[k].size();v++)
        {
            if (sequence[k][v]->cmd->run())
            {
                if (v+1 < sequence[k].size())
                {
                    sequence[k][v+1]->cmd->init();
                }
                else if (k+1 < sequence.size())
                {
                    sequence[k+1][0]->cmd->init();
                }
                else
                {
                    return true;
                }
            }
        }
    }

    return false;
}
