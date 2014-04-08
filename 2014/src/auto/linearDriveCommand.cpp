#include "WPILib.h"
#include "linearDriveCommand.hpp"
#include "../pid.hpp"
#include <math.h>
#include "../drive.hpp"

LinearDriveCommand::LinearDriveCommand(Drive *drive_, float targetDrive_, bool backwards_, float timeout_, float drivePrecision_)
{
    drive = drive_;
    targetDrive = targetDrive_;
    backwards = backwards_;
    drivePrecision = drivePrecision_;
    targetAngle = drive->getGyroAngle(); // remove compensation when we switch to colin's gyro

    setTimeout(timeout_);

    drivePID = new PID(.05, 0.001);
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
    float currGyro = drive->getGyroAngle();
    float driveError = targetDrive - drive->getWheelDistance();

    if ((timer->Get() >= timeout) || (fabs(driveError) < drivePrecision))
    {
        drive->update(0, 0, false, false, false, true);
        return true;
    }

    drivePID->setBounds(-.9, .9);
    drivePID->setTarget(targetDrive);
    anglePID->setBounds(-.5, .5);
    anglePID->setTarget(targetAngle);

    float driveInput;
    float turnInput;

    if (fabs(driveError) < drivePrecision)
    {
        driveInput = 0;
        turnInput = 0;
    }
    else
    {
        driveInput = drivePID->update(drive->getWheelDistance());
        turnInput = anglePID->update(currGyro);
    }

    drive->update(-turnInput, -driveInput, false, false, false, true);

    return false;
}
