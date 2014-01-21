#include "WPILib.h"
#include "pid.hpp"

#ifndef ARM_H
#define ARM_H

class Arm
{
public:
    Arm(Talon *motor_, Encoder *sensorA_, Encoder *sensorB, Encoder *sensorC);
    void update();
    void dashboardUpdate();
private:
    Talon *motor;
    Encoder *sensorA;
    Encoder *sensorB;
    Encoder *sensorC;

    PID *armPID;
};

#endif
