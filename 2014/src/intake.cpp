#include "WPILib.h"
#include "intake.hpp"

Intake::Intake(Arm *arm_, Victor *motor_, Solenoid *openClaw_, DigitalInput *ballSensor_)
{
    arm = arm_;
    motor = motor_;
    openClaw = openClaw_;
    ballSensor = ballSensor_;

    intakeManualSpeed = 0;
    hasBall = false;
}

float Intake::limit(float x)
{
    if (x > 1)
        return 1;
    else if (x < -1)
        return -1;
    else
        return x;
}

void Intake::manual(float speed)
{
    intakeManualSpeed = speed;
}

void Intake::update()
{
    //TODO(oliver): Switch test1 to the actual approved intaking preset
    if ((!hasBall) && (arm->getPreset() == TEST1))
    {
    }
    else if (intakeManualSpeed > 0)
    {
        motor->Set(intakeManualSpeed);
    }
    else
    {
        motor->Set(0);
    }
}

void Intake::dashboardUpdate()
{
}
