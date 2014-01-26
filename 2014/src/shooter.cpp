#include "WPILib.h"
#include "shooter.hpp"

Shooter::Shooter(Victor *winchMotor_, Solenoid *winchRelease_)
{
    winchMotor = winchMotor_;
    winchRelease = winchRelease_;
}

void Shooter::update()
{
}

void Shooter::dashboardUpdate()
{
}
