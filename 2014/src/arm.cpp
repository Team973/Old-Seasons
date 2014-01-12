#include "WPILib.h"
#include "arm.hpp"

Arm::Arm(Talon *motor_, Encoder *sensorA_, Encoder *sensorB_, Encoder *sensorC_)
{
    motor = motor_;
    sensorA = sensorA_;
    sensorB = sensorB_;
    sensorC = sensorC_;
}

void Arm::update()
{
}

void Arm::dashboardUpdate()
{
}
