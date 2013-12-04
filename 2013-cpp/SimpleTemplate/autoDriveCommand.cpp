#include "WPILib.h"
#include "AutoDriveCommand.hpp"
#include <math.h>

AutoDriveCommand::AutoDriveCommand(float targetX_,float targetY_,bool backward_,float drivePercision_,float turnPercision_,float driveCap_,float turnCap_,float arcCap_)
{

    targetX = targetX_;
    targetY = targetY_;
    backward = backward_;
    drivePercision = drivePercision_;
    turnPercision = turnPercision_;
    driveCap = driveCap_;
    turnCap = turnCap_;
    arcCap = arcCap_;

    theta = 0;
    magnitude = 0;
    deltaX = 0;
    deltaY = 0;

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
    //TODO(oliver): add the resets/calibrations for encoders + gyros
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
    currGyro = 0; //TODO(oliver): add the actual gyro
    currTheta = prevTheta + (currGyro - prevGyro);
    theta = (currTheta + prevTheta) / 2;
    //TODO(oliver): add the actual encoders
    currLeft =  0;
    currRight = 0;
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

bool AutoDriveCommand::Run()
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

    float robotLinearError = (targetY - currY) * cos(currGyro / 180 * PI) - (targetX - currX) * sin(currGyro / 180 * PI);

    float angleError = targetAngle - currGyro;

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

    if (backward)
    {
        driveInput = -driveInput;
    }

    //TODO(oliver): add drive update call here

    storeDriveCalculations();

    if (fabs(angleError) > turnPercision)
    {
        return fabs(robotLinearError) < drivePercision;
    }
    else
    {
        return false;
    }
}
