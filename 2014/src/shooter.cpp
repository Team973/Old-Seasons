#include "WPILib.h"
#include "shooter.hpp"

Shooter::Shooter(Arm *arm_, Intake *intake_, Victor *winchMotor_, Solenoid *winchRelease_, DigitalInput *zeroPoint_, DigitalInput *fullCockPoint_, Encoder *encoder_)
{
    arm = arm_;
    intake = intake_;
    winchMotor = winchMotor_;
    winchRelease = winchRelease_;
    zeroPoint = zeroPoint_;
    fullCockPoint = fullCockPoint_;
    encoder = encoder_;

    winchPID = new PID(0, 0, 0);
    winchPID->setBounds(1, -1);
    winchPID->start();

    fireTimer = new Timer();

    dangerPoint = 11;
    firing = false;
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
            currentCockPoint = fullCockPoint;
            break;
    }

    currPresetName = level;
}

void Shooter::fire(bool fire)
{
    firing = fire;
}

bool Shooter::performFire() //TODO(oliver): Add the actual arm presets
{
    fireTimer->Start();
    if (arm->getPreset() != TEST1)
    {
        intake->setFangs(true);
        if (fireTimer->Get() >= 0.5)
        {
            winchRelease->Set(false);
            firing = false;
            fireTimer->Stop();
            fireTimer->Reset();

            return true;
        }
    }
    else
        arm->setPreset(TEST2);

    return false;
}

void Shooter::update()
{
    bool fired;

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
        winchRelease->Set(true);
        if (currentCockPoint->Get())
        {
            winchMotor->Set(0);
        }
        else if (!currentCockPoint->Get() && (encoder->Get() != dangerPoint))
        {
            winchMotor->Set(winchPID->update(encoder->Get())); //TODO(oliver): Create the get distance function to pass accurate measurments to the PID
        }
    }
}

void Shooter::dashboardUpdate()
{
}
