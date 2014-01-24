#include "WPILib.h"
#include "arm.hpp"

#ifndef INTAKE_H
#define INTAKE_H

class Intake
{
public:
    Intake(Arm *arm_, Victor *motor_, Solenoid *openClaw_, DigitalInput *ballSensor_);
    void update();
    void dashboardUpdate();
    void manual(float speed);

private:
    float limit(float x);

    float intakeManualSpeed;
    bool hasBall;
    Arm *arm;
    Victor *motor;
    Solenoid *openClaw;
    DigitalInput *ballSensor;
};

#endif
