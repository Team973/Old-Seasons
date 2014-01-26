#include "WPILib.h"

#ifndef SHOOTER_H
#define SHOOTER_H

class Shooter
{
public:
    Shooter(Victor *winchMotor_, Solenoid *winchRelease_);
    void update();
    void dashboardUpdate();
private:
    Victor *winchMotor;
    Solenoid *winchRelease;
};

#endif
