#include "autoCommand.hpp"

#ifndef INTAKE_COMMAND_H
#define INTAKE_COMMAND_H

class IntakeCommand : public AutoCommand
{
public:
    IntakeCommand(Intake *intake_, Arm *arm_, float timeout_);
    virtual void Init();
    virtual bool Run();
private:
    Intake *intake;
    Arm *arm;
};

#endif
