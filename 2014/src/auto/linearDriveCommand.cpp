#include "WPILib.h"
#include "linearDriveCommand.hpp"
#include "../pid.hpp"
#include <math.h>

LinearDriveCommand::LinearDriveCommand(Drive *drive_, float targetDrive_, float targetAngle_, bool backwards_, float timeout_, float drivePrecision_, float arcPrecision_)
{
    drive = drive_;
    targetDrive = targetDrive_;
    targetAngle = targetAngle_;
    backwards = backwards_;
    drivePrecision = drivePrecision_;
    arcPrecision = arcPrecision_;

    setTimeout(timeout_);

    drivePID = new PID(.03, 0.001);
    drivePID->setICap(.3);
    anglePID = new PID(.1);
}

void LinearDriveCommand::Init()
{
    drivePID->start();
    drivePID->reset();
    anglePID->start();
    anglePID->reset();

    timer->Start();
    timer->Reset();
}

bool LinearDriveCommand::Run()
{
    if (!drivePrecision) { drivePrecision = 0; } // inches
    float currGyro = drive->getGyroAngle();
    float driveError = targetDrive - drive->getWheelDistance();

    if ((timer->Get() >= timeout) || (fabs(driveError) > drivePrecision))
    {
        drive->update(0, 0, false, false);
        return true;
    }
    else
    {
        drivePID->setBounds(-.7, .7);
        drivePID->setTarget(targetDrive);
        anglePID->setTarget(targetAngle);

        float driveInput;
        float arcInput;
        if (fabs(driveError) < drivePrecision)
        {
            driveInput = 0;
            arcInput = 0;
        }
        else
        {
            driveInput = drivePID->update(drive->getWheelDistance());
            arcInput = anglePID->update(currGyro);
        }

        drive->update(driveInput, arcInput, false, false);
    }

    return false;
}
