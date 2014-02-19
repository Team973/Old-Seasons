#include "arm.hpp"

#ifndef INTAKE_H
#define INTAKE_H

class Shooter;
class Intake
{
public:
    Intake(Arm *arm_, Shooter *shooter_, Victor *linearMotor_, Victor *crossMotor_, Solenoid *openClaw_, Solenoid *corral_, DigitalInput *ballSensor_);
    void update();
    void dashboardUpdate();
    void manual(float speed);
    void runIntake(float speed);
    void setFangs(bool state);

private:
    float limit(float x);

    float intakeManualSpeed; // manual control
    float intakeSpeed; // for automated pick up
    bool hasBall;
    Arm *arm;
    Shooter *shooter;
    Victor *linearMotor;
    Victor *crossMotor;
    Solenoid *openClaw;
    Solenoid *corral;
    DigitalInput *ballSensor;

    Timer *possesionTimer;
};

#endif
