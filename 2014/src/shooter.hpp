#include "WPILib.h"

#ifndef SHOOTER_H
#define SHOOTER_H

class Shooter
{
public:
    Shooter(Talon *winchMotor_, Solenoid *winchRelease_);
    void update();
    void dashboardUpdate();
private:
    Talon *winchMotor;
    Solenoid *winchRelease;
};

#endif
