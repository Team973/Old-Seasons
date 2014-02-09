#include "WPILib.h"
#include "pid.hpp"

#ifndef ARM_H
#define ARM_H

#define TEST1 1
#define TEST2 2

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
private:
    Talon *motor;
    Encoder *sensorA;

    PID *armPID;

    int lastPreset;
};

#endif
