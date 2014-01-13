#include "WPILib.h"
#include "intake.hpp"

Intake::Intake(Victor *motor_, Solenoid *openClaw_)
{
    motor = motor_;
    openClaw = openClaw_;
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

void Intake::manualIN(bool in)
{
    intaking = in;
}
 void Intake::manualOUT(bool out)
{
    outaking = out;
}

void Intake::update()
{
    if (intaking)
        motor->Set(1);
    else if (outaking)
        motor->Set(-1);
    else
        motor->Set(0);
}

void Intake::dashboardUpdate()
{
}
