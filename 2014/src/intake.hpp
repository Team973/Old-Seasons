#include "WPILib.h"

#ifndef INTAKE_H
#define INTAKE_H

class Intake
{
public:
    Intake(Victor *motor_, Solenoid *openClaw_);
    void update();
    void dashboardUpdate();
    void manualIN(bool in);
    void manualOUT(bool out);

private:
    float limit(float x);

    bool intaking;
    bool outaking;
    Victor *motor;
    Solenoid *openClaw;
};

#endif
