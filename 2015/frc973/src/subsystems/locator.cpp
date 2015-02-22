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

float Locator::getVelocity(Encoder *encoder) {
    float diameter = 2.5;
    float encoderTicks = 360;
    return encoder->GetRate() / encoderTicks * M_PI / 12 * diameter;
}

// in feet
float Locator::getMovedDistance()
{
    return ((getDistance(leftEncoder) + getDistance(rightEncoder))/2)/12;
}

float Locator::getLinearVelocity() {
    return (getVelocity(leftEncoder) + getVelocity(rightEncoder))/2;
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
    float encoderTicks = 1024;
    return normalizeAngle(gyro->Get() * (360 / encoderTicks));
    //return normalizeAngle(gyro->GetDegrees());
}

Locator::Point Locator::getPoint()
{
    return *currPoint;
}

void Locator::update()
{
    currPoint->angle = getAngle();
    currPoint->distance = getMovedDistance();
}

}
