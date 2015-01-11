#include "locator.hpp"
#include <math.h>
#include "../lib/utility.hpp"

namespace frc973 {

Locator::Locator(Encoder *leftEncoder_, Encoder *rightEncoder_, Encoder *gyro_)
{
    leftEncoder = leftEncoder_;
    rightEncoder = rightEncoder_;
    gyro = gyro_;

    currPoint->x = 0;
    currPoint->y = 0;

    currGyro = 0;
    currTheta = 0;
    theta = 0;
    currLeft = 0;
    currRight = 0;
    magnitude = 0;
    deltaX = 0;
    deltaY = 0;
    currX = 0;
    currY = 0;
    prevGyro = 0;
    prevTheta = 0;
    prevLeft = 0;
    prevRight = 0;
    prevX = 0;
    prevY = 0;
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

void Locator::storeCalculations()
{
    prevGyro = currGyro;
    prevTheta = currTheta;
    prevLeft = currLeft;
    prevRight = currRight;
    prevX = currX;
    prevY = currY;
}

void Locator::update()
{
    currGyro = getAngle();
    currTheta = prevTheta + (currGyro - prevGyro);
    theta = (currTheta + prevTheta)/2;
    currLeft = getDistance(leftEncoder);
    currRight = getDistance(rightEncoder);
    magnitude = (currLeft + currRight - prevLeft - prevRight) / 2;
    deltaX = -magnitude * sin(theta/180*M_PI);
    deltaY = magnitude * sin(theta/180*M_PI);
    currX = prevX + deltaX;
    currY = prevY + deltaY;

    currPoint->x = currX;
    currPoint->y = currY;
    currPoint->angle = currGyro();

    storeCalculations();
}

}
