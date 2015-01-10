#include "locator.hpp"

namespace frc973 {

Locator::Locator(Encoder *encoderX_, Encoder *encoderY_, Encoder *gyro_)
{
    encoderX = encoderX_;
    encoderY = encoderY_;
    gyro = gyro_;
}

void Locator::resetAll()
{
    encoderX->Reset();
    encoderY->Reset();
    gyro->Reset();
}

void Locator::resetGyro()
{
    gyro->Reset();
}

void Locator::resetXY()
{
    encoderX->Reset();
    encoderY->Reset();
}

// in feet
float Locator::getDistance(Encoder *encoder)
{
    float diameter = 2.5;
    float encoderTicks = 360;
    float distancePerRevolution = M_PI * diameter;
    return ((encoder->Get() / encoderTicks) * distancePerRevolution)/12;
}

float Locator::getCurrX()
{
    return getDistance(encoderX);
}

float Locator::getCurrY()
{
    return getDistance(encoderY);
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

float Locator::getAngle()
{
    float gyroTicks = 1024;
    return normalizeAngle(gyro->Get() * (360/gyroTicks));
}

void Locator::update()
{
}

}
