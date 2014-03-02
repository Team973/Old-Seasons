#include "WPILib.h"
#include "pid.hpp"
#include "arm.hpp"
#include "intake.hpp"
#include <math.h>

Arm::Arm(Talon *motor_, Encoder *sensorA_)
{

    motor = motor_;
    sensorA = sensorA_;

    armPID = new PID(0.1, 0, 0);
    armPID->setBounds(-1, 1);
    armPID->start();

    errorTarget = 1;

    armTimer = new Timer();
    armTimer->Reset();

    armMoveSpeed = 0;
}

void Arm::initialize(Intake *intake_)
{
    intake = intake_;
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
            setTarget(70.0);//96.0);
            errorTarget = 1;
            break;
        case SHOOTING:
            setTarget(42.56);
            break;
        case STOW:
            setTarget(32.00);
            break;
    }

    lastPreset = preset;
}

int Arm::getPreset()
{
    return lastPreset;
}

void Arm::ballTrapper(float magnitude)
{
    armMoveSpeed = magnitude;
}

void Arm::setTarget(float target)
{
    armPID->setTarget(target);
}

float Arm::getTarget()
{
    return armPID->getTarget();
}

bool Arm::isCockSafe()
{
    float bound = 30;
    if (getRawAngle() <= bound)
    {
        return false;
    }
    else
    {
        return true;
    }
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

    currMoveSpeed = armMoveSpeed;

    if ((lastPreset == INTAKE) || (lastPreset == PSEUDO_INTAKE))
    {
        if (intake->isClamped(true))
        intake->setFangs(false);

        if (armMoveSpeed != 0)
        {
            if ((getRawAngle() >= 70) && (getRawAngle() <= 100.0))
                motor->Set(armMoveSpeed);
            else if ((armMoveSpeed < 0 && getRawAngle() >= 100.0) || (armMoveSpeed > 0 && (getRawAngle() <= 70)))
                motor->Set(armMoveSpeed);
            else
                motor->Set(0);
        }
        else
        {
            if ((error < errorTarget && lastPreset == INTAKE) && (getRawAngle() >= 96))
            {
                errorTarget = 5; //10;
                armPID->setBounds(-0.5, 0.5);
                motor->Set(-.09);
            }
            else if (lastPreset == PSEUDO_INTAKE)
            {
                armPID->setBounds(-1, 1);
                motor->Set(armPID->update(getRawAngle()));
            }
            else
            {
                errorTarget = 1;
                motor->Set(armPID->update(getRawAngle()));
            }
        }
            if (currMoveSpeed == 0 && prevMoveSpeed != 0)
            {
                if (getRawAngle() >= 100.0)
                    setPreset(INTAKE);
                else if ((getRawAngle() <= 70))
                    setPreset(PSEUDO_INTAKE);
                else
                    setTarget(getRawAngle());
            }

    }
    else if (((lastPreset == STOW) || (lastPreset == SHOOTING)))
    {
        armPID->setBounds(-1, 1);
        armTimer->Start();
        if (!intake->isClamped())
            intake->setFangs(true);
        if (armTimer->Get() > 0.25)
        {
            motor->Set(armPID->update(getRawAngle()));
        }
        else
        {
            motor->Set(0);
        }
    }
    else
    {
        armPID->setBounds(-1, 1);
        motor->Set(armPID->update(getRawAngle()));

    }

    if (armTimer->Get() != 0 && lastPreset == (INTAKE || PSEUDO_INTAKE))
    {
        armTimer->Stop();
        armTimer->Reset();
    }

    prevMoveSpeed = currMoveSpeed;

}

void Arm::dashboardUpdate()
{
    SmartDashboard::PutNumber("Arm Angle: ", getRawAngle());
    SmartDashboard::PutNumber("Arm Error: ", fabs(getTarget() - getRawAngle()));
}
