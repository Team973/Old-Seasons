#include "WPILib.h"
#include "pid.hpp"
#include "arm.hpp"
#include <math.h>

Arm::Arm(Talon *motor_, Encoder *sensorA_)
{
    motor = motor_;
    sensorA = sensorA_;

    armPID = new PID(0.1, 0, 0);
    armPID->setBounds(-1, 1);
    armPID->start();
    ERR = false;

    errorTarget = 1;
}

void Arm::setPreset(int preset)
{
    switch (preset)
    {
        case INTAKE:
            setTarget(106.0);
            errorTarget = 1;
            break;
        case PSEUDO_INTAKE:
            setTarget(96.0);
            errorTarget = 1;
            break;
        case SHOOTING:
            setTarget(43.56);
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
    float error = fabs(getTarget() - getRawAngle());

    if ((lastPreset == INTAKE) || (lastPreset == PSEUDO_INTAKE))
    {
        if (error < errorTarget)
        {
            errorTarget = 10;
            armPID->setBounds(-0.5, 0.5);
            motor->Set(-.1);
            ERR = true;
        }
        else
        {
            errorTarget = 1;
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
