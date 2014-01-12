#include "WPILib.h"

#ifndef INTAKE_H
#define INTAKE_H

class Intake
{
public:
    Intake(Talon *motor_, Solenoid *openClaw_);
    void update();
    void dashboardUpdate();

    void manualIN(bool in);
    void manualOUT(bool out);
private:
    Talon *motor;
    Solenoid *openClaw;
};

#endif
