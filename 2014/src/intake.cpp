#include "WPILib.h"
#include "intake.hpp"

Intake::Intake(Talon *motor_, Solenoid *openClaw_)
{
    motor = motor_;
    openClaw = openClaw_;
}

void Intake::manualIN(bool in)
{
   if (in)
       motor->Set(1);
   else
       motor->Set(0);
}

void Intake::manualOUT(bool out)
{
    if (out)
        motor->Set(-1);
    else
        motor->Set(0);
}

void Intake::update()
{
}

void Intake::dashboardUpdate()
{
}
