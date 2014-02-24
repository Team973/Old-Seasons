#include "WPILib.h"
#include "pid.hpp"
#include "arm.hpp"
#include <math.h>

Arm::Arm(Talon *motor_, Encoder *sensorA_)
{
    motor = motor_;
    sensorA = sensorA_;

    armPID = new PID(0.05, 0, 0);
    armPID->setBounds(-1, 1);
    armPID->start();
    ERR = false;
}

void Arm::setPreset(int preset)
{
    switch (preset)
    {
        case INTAKE:
            setTarget(103.0);
            break;
        case SHOOTING:
            setTarget(34.56);
            break;
        case STOW:
            setTarget(68.00);
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

    if (lastPreset == INTAKE)
    {
        if (fabs(getTarget() - getRawAngle()) < 3)
        {
            armPID->setBounds(0, 0);
            motor->Set(-.2);
            ERR = true;
        }
        else
        {
            motor->Set(armPID->update(getRawAngle()));
        }
    }
    else
    {
        armPID->setBounds(-1, 1);
        motor->Set(armPID->update(getRawAngle()));
        ERR = false;
    }
}

void Arm::dashboardUpdate()
{
    SmartDashboard::PutNumber("Arm Angle: ", getRawAngle());
    SmartDashboard::PutNumber("Arm Error: ", fabs(getTarget() - getRawAngle()));
    SmartDashboard::PutBoolean("Are We Changing Stuff: ", ERR);
}
