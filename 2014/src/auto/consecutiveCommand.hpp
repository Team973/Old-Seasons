#include "autoCommand.hpp"

#ifndef CONSECUTIVECOMMAND_H
#define CONSECUTIVECOMMAND_H

class ConsecutiveCommand : public AutoCommand
{
public:
    ConsecutiveCommand(std::vector<AutoCommand*> cmd_);
    virtual void Init();
    virtual bool Run();

    // objects
    std::vector<AutoCommand*> commands;
    unsigned int commandsCompleted;
};

#endif
