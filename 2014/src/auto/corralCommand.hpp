#include "autoCommand.hpp"

#ifndef CORRAL_COMMAND_H
#define CORRAL_COMMAND_H

class Intake;
class CorralCommand : public AutoCommand
{
public:
    CorralCommand(Intake *intake);
    virtual void Init();
    virtual bool Run();
private:
    Intake *intake;
};

#endif
