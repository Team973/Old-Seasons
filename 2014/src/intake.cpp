#include "WPILib.h"
#include "intake.hpp"

Intake::Intake(Victor *motor_, Solenoid *openClaw_)
{
    motor = motor_;
    openClaw = openClaw_;
    intakeSpeed = 0;
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
    intakeSpeed = speed;
}

void Intake::update()
{
    if (intakeSpeed > 0)
    {
        motor->Set(intakeSpeed);
    }
    else
    {
        motor->Set(0);
    }
}

void Intake::dashboardUpdate()
{
}
