#include "autoCommand.hpp"

#ifndef SEQUENTIALCOMMAND_H
#define SEQUENTIALCOMMAND_H

class SequentialCommand : public AutoCommand
{
public:
    virtual void Init();
    virtual bool Run();
    SequentialCommand(std::vector<AutoCommand*> cmd_);

    // objects
    std::vector<AutoCommand*> commands;
    unsigned int commandRunning;
};

#endif
