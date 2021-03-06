#include "arm.hpp"

#ifndef INTAKE_H
#define INTAKE_H

class Shooter;
class Arm;

class Intake
{
public:
    Intake(Victor *linearMotor_, Solenoid *openClaw_, Solenoid *corral_, DigitalInput *ballSensor_);
    void initialize(Arm *arm_, Shooter *shooter_);
    void update();
    void dashboardUpdate();
    void manual(float speed);
    void runIntake(float speed);
    void setFangs(bool state, bool overide=false);
    bool isClamped();
    void stop();
    bool gotBall();
    void dropCorral(bool trap);

private:
    float limit(float x);

    float intakeManualSpeed; // manual control
    float intakeSpeed; // for automated pick up
    bool clamped;
    Arm *arm;
    Shooter *shooter;
    Victor *linearMotor;
    Solenoid *openClaw;
    Solenoid *corral;
    DigitalInput *ballSensor;

    Timer *possesionTimer;

    bool hasBall;
};

#endif
