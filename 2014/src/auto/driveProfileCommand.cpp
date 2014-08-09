#include "WPILib.h"
#include <vector>
#include "../drive.hpp"
#include "../trapProfile.hpp"
#include <math.h>
#include <vector>
#include "driveProfileCommand.hpp"

DriveProfileCommand::DriveProfileCommand(Drive *drive_, float target_, float aMax, float vMax, float dMax, float timeout_)
{
    drive = drive_;
    target = target_;

    profileGenerator = new TrapProfile(target, vMax, aMax, dMax);

    setTimeout(timeout_);
}

void DriveProfileCommand::Init()
{
    timer->Start();
    timer->Reset();
    drive->setLinear(profileGenerator);
}

bool DriveProfileCommand::Run()
{
    if (timer->Get() >= timeout)
    {
        return true;
    }

    return false;
}
