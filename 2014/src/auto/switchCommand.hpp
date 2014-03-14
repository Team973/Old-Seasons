#include "autoCommand.hpp"

#ifndef SWITCH_COMMAND_H
#define SWITCH_COMMAND_H

class SwitchCommand : public AutoCommand
{
public:
    SwitchCommand();
    virtual void Init();
    virtual bool Run();
private:
};

#endif
