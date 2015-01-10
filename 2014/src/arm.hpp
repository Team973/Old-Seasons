#include "pid.hpp"

#ifndef ARM_H
#define ARM_H

#define INTAKE 1
#define PSEUDO_INTAKE 2
#define SHOOTING 3
#define STOW 4
#define CLOSE_SHOT 5
#define HELLAVATOR 6
#define HIGH_GOAL 7

class Intake;

class Arm
{
public:
    Arm(Talon *motor_, Encoder *sensorA_, AnalogChannel *pot_);
    void initialize(Intake *intake_);
    void update();
    void dashboardUpdate();
    void setTarget(float target);
    float getTarget();
    bool isFireAngle();
    void setPreset(int preset);
    float getRawAngle();
    int getPreset();
    bool isCockSafe();
    void ballTrapper(float magnitude);
    float getError();
    void reset();
    void zeroEncoder();
    float pot2degrees();
private:
    Talon *motor;
    Encoder *sensorA;
    AnalogChannel *pot;

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
    bool zero;
    bool completeZero;
    double potZero;
    bool zeroFlag;
};

#endif
