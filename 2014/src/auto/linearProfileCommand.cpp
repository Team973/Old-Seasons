#include "WPILib.h"
#include <vector>
#include "../drive.hpp"
#include "../trapProfile.hpp"
#include <math.h>
#include <vector>
#include "linearProfileCommand.hpp"

LinearProfileCommand::LinearProfileCommand(Drive *drive_, float target_, float aMax, float vMax, float dMax, float timeout_)
{
    drive = drive_;
    target = target_;

    profileGenerator = new TrapProfile(target, vMax, aMax, dMax);

    setTimeout(timeout_);
}

void LinearProfileCommand::Init()
{
    timer->Start();
    timer->Reset();
    drive->setLinear(profileGenerator);
}

bool LinearProfileCommand::Run()
{
    if ((fabs(target - drive->getWheelDistance())*12 <= 5 && drive->getVelocity() <  2) || timer->Get() >= timeout)
    {
        return true;
    }

    return false;
}
