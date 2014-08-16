#include "autoCommand.hpp"

#ifndef CONCURRENT_COMMAND_H
#define CONCURRENT_COMMAND_H

class ConcurrentCommand : public AutoCommand
{
public:
    ConcurrentCommand(std::vector<AutoCommand*> cmd_);
    virtual void Init();
    virtual bool Run();

    // objects
    std::vector<AutoCommand*> commands;
    unsigned int commandsCompleted;
};

#endif
