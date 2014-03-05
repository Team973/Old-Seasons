#include "autoCommand.hpp"
#include "../shooter.hpp"

#ifndef FIRE_COMMAND_H
#define FIRE_COMMAND_H

class FireCommand : public AutoCommand
{
public:
    FireCommand(Shooter *shooter_, float timeout_);
    virtual void Init();
    virtual bool Run();
private:
    Shooter *shooter;
};

#endif
