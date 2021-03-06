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

    drivePID = new PID(0.03, 0, 0);//.1, 0.001);
    drivePID->setICap(.3);
    anglePID = new PID(0, 0, 0);//0.1);
    rotatePID = new PID(0, 0, 0);//.03, .02, 0.005);
    rotatePID->setICap(.1);

    driveTimer = new Timer();

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
    timer->Start();
    timer->Reset();
}

bool AutoDriveCommand::Run()
{

    float robotLinearError = 0;
    float angleError = 0;

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

    //drive->update(-turnInput, -driveInput, false, false, false, true);

    if (timer->Get() >= timeout)
    {
        //drive->update(0, 0, false, false, false, true);
        return true;
    }

    if (fabs(angleError) > turnPercision)
    {
        if (fabs(robotLinearError) < drivePercision)
        {
            //drive->update(0, 0, false, false, false, true);
            //drive->brake();
            return fabs(robotLinearError) < drivePercision;
        }
    }

    storeDriveCalculations();

    return false;
}
