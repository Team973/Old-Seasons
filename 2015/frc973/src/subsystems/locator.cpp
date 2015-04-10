#include "locator.hpp"
#include <math.h>
#include "../lib/utility.hpp"

namespace frc973 {

Locator::Locator(Encoder *leftEncoder_, Encoder *rightEncoder_, SPIGyro *gyro_, Encoder *colinGyro_)
{
    leftEncoder = leftEncoder_;
    rightEncoder = rightEncoder_;
    gyro = gyro_;
    colinGyro = colinGyro_;

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
    colinGyro->Reset();
}

void Locator::resetEncoders()
{
    leftEncoder->Reset();
    rightEncoder->Reset();
}

// in inches
float Locator::getDistance(Encoder *encoder)
{
    float diameter = 4;
    float encoderTicks = 360;
    float distancePerRevolution = M_PI * diameter;
    float gearRatio = ((12.0/54.0)*(40.0/70.0));
    return (((encoder->Get() * gearRatio) / encoderTicks) * distancePerRevolution);
}

float Locator::getVelocity(Encoder *encoder) {
    float diameter = 4;
    float encoderTicks = 360;
    float gearRatio = (12.0/54.0)*(40.0/70.0);
    return (encoder->GetRate()*gearRatio) / encoderTicks * M_PI / 12 * diameter;
}

// in feet
float Locator::getMovedDistance()
{
    //return ((getDistance(leftEncoder) + getDistance(rightEncoder))/2)/12;
    return getDistance(rightEncoder)/12;
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

float Locator::getEncoderGyro() {
    float driveWidth = 25;
    return radiansToDegrees((getDistance(rightEncoder) - getDistance(leftEncoder))/driveWidth);
}

// in degrees
float Locator::getAngle()
{
    /*
    float encoderTicks = 1024;
    return normalizeAngle(colinGyro->Get() * (360 / encoderTicks));
    */
    return gyro->GetDegrees();
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
