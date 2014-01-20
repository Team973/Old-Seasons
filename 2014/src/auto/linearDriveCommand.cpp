#include "WPILib.h"
#include "linearDriveCommand.hpp"
#include "../pid.hpp"
#include <math.h>

LinearDriveCommand::LinearDriveCommand(Drive *drive_, float targetDrive_, bool backwards_, float timeout_, float drivePrecision_)
{
    drive = drive_;
    targetDrive = targetDrive_;
    backwards = backwards_;
    drivePrecision = drivePrecision_;

    setTimeout(timeout_);

    drivePID = new PID(.03, 0.001);
    drivePID->setICap(.3);
}

void LinearDriveCommand::Init()
{
    drivePID->start();
    drivePID->reset();

    timer->Start();
    timer->Reset();
}

bool LinearDriveCommand::Run()
{
    if (!drivePrecision) { drivePrecision = 0; } // inches
    float driveError = targetDrive - drive->getWheelDistance();

    if ((timer->Get() >= timeout) || (fabs(driveError) < drivePrecision))
    {
        drive->update(0, 0, false, false);
        return true;
    }
    else
    {
        drivePID->setBounds(-.7, .7);
        drivePID->setTarget(targetDrive);

        float driveInput;
        if (fabs(driveError) < drivePrecision)
        {
            driveInput = 0;
        }
        else
        {
            driveInput = drivePID->update(drive->getWheelDistance());
        }

        if (backwards)
            driveInput = -driveInput;

        drive->update(0, -driveInput, false, false);
    }

    return false;
}
