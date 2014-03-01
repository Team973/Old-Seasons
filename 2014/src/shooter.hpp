#include "pid.hpp"
#include "intake.hpp"

#ifndef SHOOTER_H
#define SHOOTER_H

#define HALF_COCK 1
#define FULL_COCK 2
#define NO_COCK 3

class Arm;
class Shooter
{
public:
    Shooter(Arm *arm_, Intake *intake_, Victor *winchMotor_, Solenoid *winchRelease_, DigitalInput *zeroPoint, DigitalInput *fullCockPoint_, Encoder *encoder_);
    void cock(int level); // Yes, laugh all you want. It's very funny.
    void fire(bool fire);
    bool isFiring();
    bool performFire();
    void killShooter(bool dead);
    void update();
    void dashboardUpdate();

    void testWinch(float speed);
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
    Encoder *encoder;

    Timer *fireTimer;
    Timer *cockTimer;

    int currPresetName;
    bool firing;
    float dangerPoint;
    float M_PI;
    bool currZeroPoint;
    bool prevZeroPoint;

    float winchSpeed;
};

#endif
