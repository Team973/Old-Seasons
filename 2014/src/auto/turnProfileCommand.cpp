#include "WPILib.h"
#include "turnProfileCommand.hpp"
#include "../drive.hpp"
#include "../trapProfile.hpp"
#include <math.h>

TurnProfileCommand::TurnProfileCommand(Drive *drive_, float target_, float aMax, float vMax, float dMax, float timeout_)
{
    drive = drive_;
    target = target_;
    profileGenerator = new TrapProfile(target, vMax, aMax, dMax);

    setTimeout(timeout_);
}

void TurnProfileCommand::Init()
{
    timer->Start();
    timer->Reset();
    drive->setAngular(profileGenerator);
}

bool TurnProfileCommand::Run()
{
    if ((fabs(target - drive->getGyroAngle()) <= 5) || timer->Get() >= timeout)
    {
        return true;
    }

    return false;
}
