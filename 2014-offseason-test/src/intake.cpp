#include "WPILib.h"
#include "intake.hpp"

Intake::Intake(Victor *motor_, Solenoid *claw_)
{
    motor = motor_;
    claw = claw_;
}

void Intake::setBehavior(float speed, bool open)
{
    setSpeed(speed);
    setFangs(open);
}

void Intake::setFangs(bool open)
{
    claw->Set(open);
}

void Intake::setSpeed(float speed)
{
    intakeSpeed = speed;
}

void Intake::update()
{
    motor->Set(intakeSpeed);
}
