#include "WPILib.h"
#include "consecutiveCommand.hpp"
#include <vector>

ConsecutiveCommand::ConsecutiveCommand(std::vector<AutoCommand*> cmd_)
{
    for (unsigned int i=0; i<cmd_.size(); i++)
    {
        commands.push_back(cmd_[i]);
    }

    commandsCompleted = 0;
}

void ConsecutiveCommand::Init()
{
    for (unsigned int i=0; i<commands.size(); i++)
    {
        commands[i]->Init();
    }
}

bool ConsecutiveCommand::Run()
{
    if (commandsCompleted != commands.size())
    {
        for (unsigned int i=0; i<commands.size(); i++)
        {
            if (commands[i]->Run())
            {
                commandsCompleted += 1;
            }
        }
    }
    else
        return true;

    return false;
}
