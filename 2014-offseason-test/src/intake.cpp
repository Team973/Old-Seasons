#include "WPILib.h"
#include "intake.hpp"

namespace frc973 {

Intake::Intake(Victor *motor_, Solenoid *claw_)
{
    motor = motor_;
    claw = claw_;
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

}
