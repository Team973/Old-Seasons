#include "pid.hpp"

#ifndef ARM_H
#define ARM_H

#define INTAKE 1
#define PSEUDO_INTAKE 2
#define SHOOTING 3
#define STOW 4
#define CLOSE_SHOT 5

class Intake;

class Arm
{
public:
    Arm(Talon *motor_, Encoder *sensorA_);
    void initialize(Intake *intake_);
    void update();
    void dashboardUpdate();
    void setTarget(float target);
    float getTarget();
    void setPreset(int preset);
    float getRawAngle();
    int getPreset();
    bool isCockSafe();
    void ballTrapper(float magnitude);
    float getError();
private:
    Talon *motor;
    Encoder *sensorA;

    Intake *intake;

    PID *armPID;

    Timer *armTimer;

    int lastPreset;

    float errorTarget;

    float armMoveSpeed;

    float currMoveSpeed;
    float prevMoveSpeed;
    float error;
    bool autoClamped;
};

#endif
