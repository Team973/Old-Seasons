#include "WPILib.h"
#include "concurrentCommand.hpp"
#include <vector>

ConcurrentCommand::ConcurrentCommand(std::vector<AutoCommand*> cmd_)
{
    for (unsigned int i=0; i<cmd_.size(); i++)
    {
        commands.push_back(cmd_[i]);
    }

    commandsCompleted = 0;
}

void ConcurrentCommand::Init()
{
    for (unsigned int i=0; i<=commands.size(); i++)
    {
        commands[i]->Init();
    }
}

bool ConcurrentCommand::Run()
{
    if (commandsCompleted < commands.size())
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
