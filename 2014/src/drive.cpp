#include "WPILib.h"
#include "drive.hpp"
#include <math.h>

Drive::Drive(Talon *leftDrive_, Talon *rightDrive_, Solenoid *shifters_, Solenoid *kickUp_, Encoder *leftEncoder_, Encoder *rightEncoder_)
{
    leftDrive = leftDrive_;
    rightDrive = rightDrive_;

    shifters = shifters_;
    kickUp = kickUp_;

    leftEncoder = leftEncoder_;
    rightEncoder = rightEncoder_;

    M_PI = 3.141592;


}

float Drive::limit(float x)
{
    if (x > 1)
        return 1;
    else if (x < -1)
        return -1;

    return x;
}

void Drive::setDriveMotors(float left, float right)
{
    leftDrive->Set(limit(left));
    rightDrive->Set(-limit(right));
}

float Drive::getLeftDrive()
{
    return leftEncoder->Get();
}

float Drive::getRightDrive()
{
    return rightEncoder->Get();
}

float Drive::getWheelDistance()
{
    float diameter = 2.75;
    float encoderTicks = 360;
    float distancePerRevolution = M_PI * diameter;
    leftDist = (leftEncoder->Get() / encoderTicks) * distancePerRevolution;
    rightDist = (rightEncoder->Get() / encoderTicks) * distancePerRevolution;
    return (leftDist + rightDist)/2;
}

float Drive::normalizeAngle(float theta)
{
    while (theta > 180)
    {
        theta -= 360;
    }
    while (theta < -180)
    {
        theta += 360;
    }

    return theta;
}

float Drive::getGyroAngle()
{
    return normalizeAngle(0); //TODO(oliver): Add the new gyro get here
}

void Drive::resetDriveEncoders()
{
    leftEncoder->Reset();
    rightEncoder->Reset();
}

float Drive::getLeftDistance()
{
    return leftDist;
}

float Drive::getRightDistance()
{
    return rightDist;
}

float Drive::signSquare(float x)
{
    if (x < 0)
        return -x*x;
    return x*x;
}

void Drive::arcade(float move_, float rotate_)
{
    float move = signSquare(limit(move_));
    float rotate = signSquare(limit(rotate_));
    float left, right;
    if (move < 0)
    {
        if (rotate > 0)
        {
            left = (-move - rotate);
            right = max(-move, rotate);
        }
        else
        {
            left = max(-move, -rotate);
            right = (-move + rotate);
        }
    }
    else
    {
        if (rotate > 0)
        {
            left = -max(move, rotate);
            right = (-move + rotate);
        }
        else
        {
            left = (-move - rotate);
            right = -max(move, -rotate);
        }
    }
    setDriveMotors(left, right);
}

void Drive::setLowGear(bool lowGear)
{
    shifters->Set(lowGear);
}

void Drive::setKickUp(bool kick)
{
    kickUp->Set(kick);
}

void Drive::update(double DriveX, double DriveY, bool gear, bool kick, bool quickTurn)
{
    arcade(DriveY, DriveX);
    setLowGear(gear);
    setKickUp(kick);
}

void Drive::dashboardUpdate()
{
    SmartDashboard::PutNumber("Left Encoder RAW Ticks", leftEncoder->Get());
    SmartDashboard::PutNumber("Right Encoder RAW Ticks", rightEncoder->Get());
}

void Drive::resetDrive()
{
    resetDriveEncoders();
}
