#include "WPILib.h"
#include "linearDriveCommand.hpp"
#include "../pid.hpp"
#include <math.h>
#include "../drive.hpp"

LinearDriveCommand::LinearDriveCommand(Drive *drive_, float targetDrive_, float targetAngle_, bool backwards_, float timeout_)
{
    drive = drive_;
    targetDrive = targetDrive_;
    backwards = backwards_;
    if (targetAngle_ == 0)
        targetAngle = drive->getGyroAngle();
    else
        targetAngle = targetAngle_;

    setTimeout(timeout_);

    drivePID = new PID(.02, 0, 0.08);
    anglePID = new PID(.05, 0, 0.05);

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
    float angleError = targetAngle - currGyro;

    if ((timer->Get() >= timeout) || (fabs(driveError) <= 0.1))
    {
        drive->update(0, 0, false, false, false, true);
        return true;
    }

    drivePID->setBounds(-.9, .9);
    anglePID->setBounds(-.7, .7);

    float driveInput;
    float turnInput;

    driveInput = driveError;
    turnInput = angleError;

    SmartDashboard::PutNumber("Drive Error: ", driveError);
    SmartDashboard::PutNumber("Angle Error: ", angleError);

    drive->update(turnInput, driveInput, false, false, false, true);

    return false;
}
