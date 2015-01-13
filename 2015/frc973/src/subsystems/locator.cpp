#include "locator.hpp"
#include <math.h>
#include "../lib/utility.hpp"

namespace frc973 {

Locator::Locator(Encoder *leftEncoder_, Encoder *rightEncoder_, Encoder *gyro_)
{
    leftEncoder = leftEncoder_;
    rightEncoder = rightEncoder_;
    gyro = gyro_;

    currPoint = new Point;
}

void Locator::resetAll()
{
    leftEncoder->Reset();
    rightEncoder->Reset();
    gyro->Reset();
}

void Locator::resetGyro()
{
    gyro->Reset();
}

void Locator::resetEncoders()
{
    leftEncoder->Reset();
    rightEncoder->Reset();
}

// in inches
float Locator::getDistance(Encoder *encoder)
{
    float diameter = 2.5;
    float encoderTicks = 360;
    float distancePerRevolution = M_PI * diameter;
    return ((encoder->Get() / encoderTicks) * distancePerRevolution);
}

// in feet
float Locator::getMovedDistance()
{
    return ((getDistance(leftEncoder) + getDistance(rightEncoder))/2)/12;
}

float Locator::normalizeAngle(float theta)
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

// in degrees
float Locator::getAngle()
{
    float gyroTicks = 1024;
    return normalizeAngle(gyro->Get() * (360/gyroTicks));
}

Locator::Point* Locator::getPoint()
{
    return currPoint;
}

void Locator::update()
{
    currPoint->angle = getAngle();
    currPoint->distance = getMovedDistance();
}

}
