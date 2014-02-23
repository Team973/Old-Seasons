#include "WPILib.h"
#include "pid.hpp"
#include "arm.hpp"

Arm::Arm(Talon *motor_, Encoder *sensorA_)
{
    motor = motor_;
    sensorA = sensorA_;

    armPID = new PID(0.05, 0, 0);
    armPID->setBounds(-1, 1);
    armPID->start();
}

void Arm::setPreset(int preset)
{
    switch (preset)
    {
        case INTAKE:
            setTarget(102.09);
            break;
        case SHOOTING:
            setTarget(34.56);
            break;
    }

    lastPreset = preset;
}

int Arm::getPreset()
{
    return lastPreset;
}

void Arm::setTarget(float target)
{
    armPID->setTarget(target);
}

float Arm::getTarget()
{
    return armPID->getTarget();
}

float Arm::getRawAngle()
{
    float degreesPerRevolution = 360;
    float gearRatio = 1;
    float ticksPerRevolution = 2500;
    return sensorA->Get() / (ticksPerRevolution * gearRatio) * degreesPerRevolution;
}

void Arm::update()
{
    motor->Set(armPID->update(getRawAngle()));
}

void Arm::dashboardUpdate()
{
    SmartDashboard::PutNumber("Arm Angle: ", getRawAngle());
}
