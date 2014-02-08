#include "WPILib.h"
#include "pid.hpp"

#ifndef SHOOTER_H
#define SHOOTER_H

class Shooter
{
public:
    Shooter(Victor *winchMotor_, Solenoid *winchRelease_, DigitalInput *zeroPoint, DigitalInput *fullCockPoint_, Encoder *encoder_);
    void update();
    void dashboardUpdate();
private:
    PID *winchPID;

    Victor *winchMotor;
    Solenoid *winchRelease;
    DigitalInput *zeroPoint;
    DigitalInput *fullCockPoint;
    Encoder *encoder;
};

#endif
