#include "WPILib.h"
#include "../intake.hpp"
#include "../arm.hpp"
#include "intakeCommand.hpp"

IntakeCommand::IntakeCommand(Intake *intake_, Arm *arm_, float timeout_)
{
    intake = intake_;
    arm = arm_;
    setTimeout(timeout_);
}

void IntakeCommand::Init()
{
    timer->Start();
    timer->Reset():
}

bool IntakeCommand::Run()
{
    return true;
}
