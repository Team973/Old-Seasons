#include "WPILib.h"
#include "pid.hpp"
#include "arm.hpp"
#include "intake.hpp"

#ifndef SHOOTER_H
#define SHOOTER_H

#define HALF_COCK 1
#define FULL_COCK 2

class Shooter
{
public:
    Shooter(Arm *arm_, Intake *intake_, Victor *winchMotor_, Solenoid *winchRelease_, DigitalInput *zeroPoint, DigitalInput *fullCockPoint_, Encoder *encoder_);
    void cock(int level); // Yes laugh all you want. It's very funny
    void fire(bool fire);
    void manualCock(bool running);
    void manualFire();
    bool performFire();
    void update();
    void dashboardUpdate();
private:
    void setTarget(float target);
    float winchDistance();

    Arm *arm;
    Intake *intake;
    PID *winchPID;

    Victor *winchMotor;
    Solenoid *winchRelease;
    DigitalInput *zeroPoint;
    DigitalInput *fullCockPoint;
    DigitalInput *currentCockPoint;
    Encoder *encoder;

    Timer *fireTimer;

    int currPresetName;
    bool firing;
    float dangerPoint;
    float M_PI;
    bool currZeroPoint;
    bool prevZeroPoint;
    bool motorRunning;
    bool manual;
};

#endif
