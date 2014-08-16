#include "WPILib.h"
#include "../TrapProfile.hpp"
#include "../drive.hpp"
#include "driveProfileCommand.hpp"

DriveProfileCommand::DriveProfileCommand(Drive *drive_, float linearTarget_, float angularTarget_, float timeout_)
{
    drive = drive_;
    linearTarget = linearTarget_; // feet
    angularTarget = angularTarget_; // degrees

    setTimeout(timeout_);
}

void DriveProfileCommand::Init()
{
    timer->Start();
    timer->Reset();
}

bool DriveProfileCommand::Run()
{
    return false;
}
