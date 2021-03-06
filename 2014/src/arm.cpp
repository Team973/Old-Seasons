#include "WPILib.h"
#include "pid.hpp"
#include "arm.hpp"
#include "intake.hpp"
#include <math.h>

Arm::Arm(Talon *motor_, Encoder *sensorA_, AnalogChannel *pot_)
{

    motor = motor_;
    sensorA = sensorA_;
    pot = pot_;

    armPID = new PID(0.12, 0, 0);
    armPID->setBounds(-1, 1);
    armPID->start();

    errorTarget = 1;

    potZero = pot->GetVoltage();

    armTimer = new Timer();
    armTimer->Reset();

    armMoveSpeed = 0;

    error = 0;
    autoClamped = false;
    zero = false;
    completeZero = false;
    zeroFlag = false;
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
            setTarget(107.0);
            errorTarget = 1;
            break;
        case PSEUDO_INTAKE:
            setTarget(67.0);//96.0);
            errorTarget = 1;
            break;
        case SHOOTING:
            setTarget(26.5);//27.5);
            autoClamped = false;
            break;
        case STOW:
            setTarget(29.00);
            autoClamped = false;
            break;
        case CLOSE_SHOT:
            setTarget(8);
            autoClamped = false;
            break;
        case HELLAVATOR:
            setTarget(7.5);//2.5
            break;
        case HIGH_GOAL:
            setTarget(22);//27.5);
            autoClamped = false;
            break;
    }

    lastPreset = preset;
}

int Arm::getPreset()
{
    return lastPreset;
}

bool Arm::isFireAngle()
{
    if (lastPreset == SHOOTING || lastPreset == CLOSE_SHOT || lastPreset == HIGH_GOAL || lastPreset == STOW)
        return true;

    return false;
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
    float bound = 0;//30;
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

float Arm::pot2degrees()
{
    float multiplier = (112.01 - 0.00)/(1.87 - 5.01);
    return (pot->GetVoltage() - potZero)*multiplier;
}

float Arm::getError()
{
    return error;
}

void Arm::reset()
{
    zero = true;
}

void Arm::zeroEncoder()
{
    completeZero = true;
}

void Arm::update()
{
    error = fabs(getTarget() - getRawAngle());

    currMoveSpeed = armMoveSpeed;

    if (zero)
    {
        motor->Set(-.5);
        if (sensorA->GetRate() > 5)
            zeroFlag = true;

        if (zeroFlag && sensorA->GetRate() < 2)
        {
            sensorA->Reset();
            zeroFlag = false;
            zero = false;
        }
    }
    else if ((lastPreset == INTAKE) || (lastPreset == PSEUDO_INTAKE))
    {
        autoClamped = false;
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
                motor->Set(-.06);
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
    else if (((lastPreset == STOW) || (lastPreset == SHOOTING || lastPreset == CLOSE_SHOT || lastPreset == HIGH_GOAL)))
    {
        armPID->setBounds(-1, 1);

        if (!intake->isClamped() && armTimer->Get() == 0 && !autoClamped)
        {
            intake->setFangs(true, false);
            autoClamped = true;
            armTimer->Start();
        }


        if ((armTimer->Get() > 0.25))
        {
            motor->Set(armPID->update(getRawAngle()));

            if (armTimer->Get() != 0)
            {
                armTimer->Stop();
                armTimer->Reset();
            }
        }
        else if (armTimer->Get() < 0.25 && armTimer->Get() > 0 )
            motor->Set(0);
        else
        {
            motor->Set(armPID->update(getRawAngle()));
        }
    }
    else
    {
        armPID->setBounds(-1, 1);
        motor->Set(armPID->update(getRawAngle()));

    }

    prevMoveSpeed = currMoveSpeed;

}

void Arm::dashboardUpdate()
{
    //SmartDashboard::PutNumber("Arm Angle: ", getRawAngle());
}
