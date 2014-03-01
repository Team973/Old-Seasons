#include "pid.hpp"

#ifndef ARM_H
#define ARM_H

#define INTAKE 1
#define PSEUDO_INTAKE 2
#define SHOOTING 3
#define STOW 4

class Arm
{
public:
    Arm(Talon *motor_, Encoder *sensorA_);
    void update();
    void dashboardUpdate();
    void setTarget(float target);
    float getTarget();
    void setPreset(int preset);
    float getRawAngle();
    int getPreset();
    bool isCockSafe();
private:
    Talon *motor;
    Encoder *sensorA;

    PID *armPID;

    int lastPreset;

    float errorTarget;
};

#endif
