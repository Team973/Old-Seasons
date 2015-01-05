#include "locator.hpp"

namespace frc973 {

Locator::Locator(Encoder *encoderX_, Encoder *encoderY_, Encoder *gyro_)
{
    encoderX = encoderX_;
    encoderY = encoderY_;
    gyro = gyro_;
}

void Locator::reset()
{
    encoderX->Reset();
    encoderY->Reset();
    gyro->Reset();
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

void Locator::update()
{
}

}
