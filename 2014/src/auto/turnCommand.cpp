#include "WPILib.h"
#include "../drive.hpp"
#include "turnCommand.hpp"
#include "../pid.hpp"
#include <math.h>

TurnCommand::TurnCommand(Drive* drive_, float targetAngle_, float timeout_, float turnPrecision_, float turnCap_)
{
    setTimeout(timeout_);

    drive = drive_;
    targetAngle = targetAngle_;
    turnPrecision = turnPrecision_;
    turnCap = turnCap_;

    turnInput = 0;

    rotatePID = new PID(.01, .01, 0.005);
    rotatePID->setICap(.1);
}

void TurnCommand::Init()
{
    rotatePID->start();
    rotatePID->reset();

    timer->Start();
    timer->Reset();
}

bool TurnCommand::Run()
{
    float currGyro = drive->getGyroAngle();
    float angleError = targetAngle - currGyro;

    rotatePID->setBounds(-turnCap, turnCap);

    if (fabs(angleError) < turnPrecision)
        turnInput = 0;
    else
    {
        SmartDashboard::PutBoolean("Turn End Condition: ", false);
        turnInput = rotatePID->update(angleError);
    }

    drive->update(turnInput, 0, false, false, true);

    if ((fabs(angleError) < turnPrecision) || (timer->Get() >= timeout))
    {
        SmartDashboard::PutBoolean("Turn End Condition: ", true);
        drive->update(0, 0, false, false, true);
        drive->brake();
        return true;
    }

    return false;
}
