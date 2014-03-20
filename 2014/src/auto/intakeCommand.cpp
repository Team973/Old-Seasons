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
    timer->Reset();

    arm->setPreset(INTAKE);
}

bool IntakeCommand::Run()
{
    if (intake->gotBall() || timer->Get() >= timeout)
    {
        arm->setPreset(SHOOTING);
        return true;
    }
    else
    {
        intake->runIntake(-1);
        return false;
    }
}
