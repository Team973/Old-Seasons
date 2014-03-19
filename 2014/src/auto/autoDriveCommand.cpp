#include "WPILib.h"
#include "autoDriveCommand.hpp"
#include "../drive.hpp"
#include <math.h>

AutoDriveCommand::AutoDriveCommand(Drive* drive_, float targetX_,float targetY_,bool backward_,double timeout_,float drivePercision_,float turnPercision_,float driveCap_,float turnCap_,float arcCap_)
{

    setTimeout(timeout_);

    drive = drive_;

    targetX = targetX_;
    targetY = targetY_;
    backward = backward_;
    drivePercision = drivePercision_;
    turnPercision = turnPercision_;
    driveCap = driveCap_;
    turnCap = turnCap_;
    arcCap = arcCap_;

    drivePID = new PID(.03, 0.001);
    drivePID->setICap(.3);
    anglePID = new PID(.1);
    rotatePID = new PID(.15, .01, 0.005);
    rotatePID->setICap(.1);

    PI = 3.14159;

    resetDrive();
}

void AutoDriveCommand::resetDrive()
{
    prevTheta = 0;
    currTheta = 0;
    prevGyro = 0;
    currGyro = 0;
    prevLeft = 0;
    currLeft = 0;
    prevRight = 0;
    currRight = 0;
    prevX = 0;
    currX = 0;
    prevY = 0;
    currY = 0;

    drivePID->reset();
    anglePID->reset();
    rotatePID->reset();

    drivePID->start();
    anglePID->start();
    rotatePID->start();
}

void AutoDriveCommand::calculateDrive()
{
    currGyro = drive->getGyroAngle();
    currTheta = prevTheta + (currGyro - prevGyro);
    theta = (currTheta + prevTheta) / 2;
    currLeft =  drive->getLeftDistance();
    currRight = drive->getRightDistance();
    magnitude = (currLeft + currRight - prevLeft - prevRight) / 2;
    deltaX = -magnitude * sin(theta / 180 * PI);
    deltaY = magnitude * cos(theta / 180 * PI);
    currX = prevX + deltaX;
    currY = prevY + deltaY;
}

void AutoDriveCommand::storeDriveCalculations()
{
    prevGyro = currGyro;
    prevTheta = currTheta;
    prevLeft = currLeft;
    prevRight = currRight;
    prevX = currX;
    prevY = currY;
}

void AutoDriveCommand::Init()
{
    drive->holdPosition(false, 0, 0, 0);
    resetDrive();
    timer->Start();
    timer->Reset();
}

bool AutoDriveCommand::Run()
{
    float robotLinearError = 0;
    float angleError = 0;

    if ((timer->Get() >= timeout) || ((fabs(robotLinearError) < drivePercision) && (fabs(angleError) > turnPercision)))
    {
        drive->update(0, 0, false, false, false, true);
        drive->holdPosition(true, drivePID->getTarget(), drivePercision, turnPercision);
        return true;
    }
    else
    {
        if (!drivePercision) { drivePercision = 6;} // inches
        if (!turnPercision) { turnPercision = 5;} // degrees

        if (!driveCap) { driveCap = 0.5;}
        if (!arcCap) { arcCap = 0.3;}
        if (!turnCap) { turnCap = 0.7;}

        drivePID->setBounds(-driveCap, driveCap);
        anglePID->setBounds(-arcCap, arcCap);
        rotatePID->setBounds(-turnCap, turnCap);

        calculateDrive();

        float driveError = sqrt(pow((targetX - currX), 2) + pow((targetY - currY), 2));
        float targetAngle = atan2(currX - targetX, targetY - currY) / PI * 180;
        if (backward)
            targetAngle = targetAngle + 180;
        while (targetAngle > 180)
            targetAngle = targetAngle - 360;

        robotLinearError = (targetY - currY) * cos(currGyro / 180 * PI) - (targetX - currX) * sin(currGyro / 180 * PI);

        angleError = targetAngle - currGyro;

        float driveInput = 0;
        float turnInput = 0;

        if (fabs(angleError) < turnPercision)
        {
            if (fabs(robotLinearError) < drivePercision)
            {
                driveInput = turnInput = 0;
            }
            else
            {
                driveInput = -drivePID->update(driveError);
                turnInput = anglePID->update(angleError);
            }

        }
        else
        {
            driveInput = 0;
            turnInput = rotatePID->update(angleError); // only this one gets a pid output
        }

        /*
        if (backward)
        {
            driveInput = -driveInput;
        }
        */

        drive->update(driveInput, turnInput, true, false, false, true);

        storeDriveCalculations();

    }
    return false;
}
