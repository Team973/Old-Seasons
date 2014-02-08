#include "WPILib.h"
#include "shooter.hpp"

Shooter::Shooter(Victor *winchMotor_, Solenoid *winchRelease_, DigitalInput *zeroPoint_, DigitalInput *fullCockPoint_, Encoder *encoder_)
{
    winchMotor = winchMotor_;
    winchRelease = winchRelease_;
    zeroPoint = zeroPoint_;
    fullCockPoint = fullCockPoint_;
    encoder = encoder_;

    winchPID = new PID(0, 0, 0);
    winchPID->setBounds(1, -1);
    winchPID->start();
}

void Shooter::update()
{
}

void Shooter::dashboardUpdate()
{
}
