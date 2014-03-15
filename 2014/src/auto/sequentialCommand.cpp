#include "WPILib.h"
#include <vector>
#include "sequentialCommand.hpp"

SequentialCommand::SequentialCommand(std::vector<AutoCommand*> cmd_)
{
    for ( unsigned int i=0; i<cmd_.size(); i++)
    {
        commands.push_back(cmd_[i]);
    }
    commandRunning = 0;
}

int SequentialCommand::getCurrentCommand()
{
    return commandRunning;
}

void SequentialCommand::Init()
{
    commands[0]->Init();
}

bool SequentialCommand::Run()
{
    if (commandRunning < commands.size())
    {
        if (commands[commandRunning]->Run())
        {
            commandRunning += 1;
            if (commandRunning >= commands.size())
                return true;
            else
                commands[commandRunning]->Init();
        }
        else
            return false;
    }
    else
        return true;

    return false;
}
