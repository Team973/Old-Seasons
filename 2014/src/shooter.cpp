#include "WPILib.h"
#include "shooter.hpp"

Shooter::Shooter(Arm *arm_, Intake *intake_, Victor *winchMotor_, Solenoid *winchRelease_, DigitalInput *zeroPoint_, DigitalInput *fullCockPoint_, Encoder *encoder_)
{
    arm = arm_;
    intake = intake_;
    winchMotor = winchMotor_;
    winchMotor->Set(0);
    winchRelease = winchRelease_;
    zeroPoint = zeroPoint_;
    fullCockPoint = fullCockPoint_;
    encoder = encoder_;

    winchPID = new PID(100000, 0, 0);
    winchPID->setBounds(0, 1);
    winchPID->start();

    fireTimer = new Timer();

    dangerPoint = 10.5;
    firing = false;
    fired = false;

    M_PI = 3.141592;

    currZeroPoint = false;
    prevZeroPoint = false;

    STOP = false;
}

void Shooter::setTarget(float target)
{
    winchPID->setTarget(target);
}

void Shooter::cock(int level)
{
    switch (level)
    {
        case HALF_COCK:
            setTarget(5);
            break;
        case FULL_COCK:
            setTarget(10);
            break;
    }

    currPresetName = level;
}

void Shooter::fire(bool fire)
{
    firing = fire;
}

bool Shooter::isFiring()
{
    return firing;
}

float Shooter::winchDistance()
{
    float encoderTicks = 360;
    float diameter = 1.4;
    float gearRatio = 2;
    float distancePerRevolution = diameter * M_PI;
    return (encoder->Get() * distancePerRevolution) / (encoderTicks * gearRatio);
}

bool Shooter::performFire()
{
    if (arm->getPreset() != INTAKE)
    {
        fireTimer->Start();
        intake->setFangs(true);
        intake->runIntake(.5);
        if (fireTimer->Get() >= 0.5)
        {
            winchRelease->Set(true);
            fireTimer->Stop();
            fireTimer->Reset();
            intake->runIntake(0);
            firing = false;

            return true;
        }
    }

    return false;
}

void Shooter::update()
{
    currZeroPoint = zeroPoint->Get();
    if ((currZeroPoint) && (!prevZeroPoint))
    {
        encoder->Reset();
    }
    else if ((!currZeroPoint) && (prevZeroPoint))
    {
        encoder->Reset();
    }

    if (firing)
    {
        fired = false;

        if (performFire())
        {
            fired = true;
        }
    }
    else if (fired)
    {
        cock(HALF_COCK);
        fired = false;
    }
    else
    {
        winchRelease->Set(false);
        if ((!fullCockPoint->Get()) || (winchDistance() >= dangerPoint))
        {
            winchMotor->Set(0);
            STOP = true;
        }
        else if (fullCockPoint->Get() && (winchDistance() < dangerPoint))
        {
            if (winchDistance() < winchPID->getTarget())
            {
            winchMotor->Set(winchPID->update(winchDistance()));
            }
            else
            {
            winchMotor->Set(0);
            }
            STOP = false;
        }
    }

    prevZeroPoint = currZeroPoint;

}

void Shooter::dashboardUpdate()
{
    SmartDashboard::PutNumber("Winch Encoder Distance: ", winchDistance());
    SmartDashboard::PutBoolean("Zero Hall Effects: ", zeroPoint->Get());
    SmartDashboard::PutBoolean("Full Cock Hall Effects: ", fullCockPoint->Get());
    SmartDashboard::PutBoolean("STOP: ", STOP);
}
