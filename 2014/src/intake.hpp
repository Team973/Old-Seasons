#include "WPILib.h"

#ifndef INTAKE_H
#define INTAKE_H

class Intake
{
public:
    Intake(Victor *motor_, Solenoid *openClaw_);
    void update();
    void dashboardUpdate();
    void manual(float speed);

private:
    float limit(float x);

    float intakeSpeed;
    Victor *motor;
    Solenoid *openClaw;
};

#endif
