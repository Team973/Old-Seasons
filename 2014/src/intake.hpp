#include "WPILib.h"
#include "arm.hpp"

#ifndef INTAKE_H
#define INTAKE_H

class Intake
{
public:
    Intake(Arm *arm_, Victor *linearMotor_, Victor *crossMotor_, Solenoid *openClaw_, DigitalInput *ballSensor_);
    void update();
    void dashboardUpdate();
    void manual(float speed);
    void runIntake(float speed);

private:
    float limit(float x);

    float intakeManualSpeed; // manual control
    float intakeSpeed; // for automated pick up
    bool hasBall;
    Arm *arm;
    Victor *linearMotor;
    Victor *crossMotor;
    Solenoid *openClaw;
    DigitalInput *ballSensor;

    Timer *possesionTimer;
};

#endif
