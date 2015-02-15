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

    rotatePID = new PID(.005, 0, 0.01);
}

void TurnCommand::Init()
{
    timer->Start();
    timer->Reset();
}

bool TurnCommand::Run()
{

    /*
    if ((fabs(angleError) < turnPrecision) || (timer->Get() >= timeout))
    {
        return true;
    }
    */

    return false;
}
