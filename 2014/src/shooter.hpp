#include "pid.hpp"
#include "intake.hpp"

#ifndef SHOOTER_H
#define SHOOTER_H

#define HALF_COCK 1
#define FULL_COCK 2
#define NO_COCK 3

class Arm;
class Intake;

class Shooter
{
public:
    Shooter(Victor *winchMotor_, Victor *trussWinch_, Solenoid *winchRelease_, DigitalInput *zeroPoint, DigitalInput *fullCockPoint_, AnalogChannel *Pot_, Encoder *encoder_);
    void initialize(Arm *arm_, Intake *intake_);
    void cock(int level); // Yes, laugh all you want. It's very funny.
    void setTrussTarget(float target);
    void setTruss();
    void setDeTruss();
    void fire(bool fire);
    bool isFiring();
    bool performFire();
    bool performSoftFire();
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
    PID *trussPID;

    Victor *winchMotor;
    Victor *trussWinch;
    Solenoid *winchRelease;
    DigitalInput *zeroPoint;
    DigitalInput *fullCockPoint;
    Encoder *encoder;
    AnalogChannel *Pot;

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
