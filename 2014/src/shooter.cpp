#include "WPILib.h"
#include "shooter.hpp"
#include "pid.hpp"
#include "arm.hpp"
#include "intake.hpp"
#include <math.h>

Shooter::Shooter(Victor *winchMotor_, Victor *trussWinch_, Solenoid *winchRelease_, DigitalInput *zeroPoint_, DigitalInput *fullCockPoint_, AnalogChannel *Pot_, Encoder *encoder_)
{
    winchMotor = winchMotor_;
    winchMotor->Set(0);
    trussWinch = trussWinch_;
    trussWinch->Set(0);
    winchRelease = winchRelease_;
    zeroPoint = zeroPoint_;
    fullCockPoint = fullCockPoint_;
    Pot = Pot_;
    encoder = encoder_;

    winchPID = new PID(100000, 0, 0);
    winchPID->setBounds(0, 1);
    winchPID->start();

    trussPID = new PID(1.8, 0, 0);
    trussPID->setBounds(-1, 1);
    trussPID->start();
    setDeTruss();

    fireTimer = new Timer();
    cockTimer = new Timer();

    dangerPoint = 11;
    firing = false;

    M_PI = 3.141592;

    currZeroPoint = false;
    prevZeroPoint = false;

    winchSpeed = 0;

}

void Shooter::initialize(Arm *arm_, Intake *intake_)
{
    arm = arm_;
    intake = intake_;
}

void Shooter::setTarget(float target)
{
    winchPID->setTarget(target);
}

void Shooter::setTrussTarget(float target)
{
    trussPID->setTarget(target);
}

void Shooter::setTruss()
{
    setTrussTarget(1.4);
}

void Shooter::setDeTruss()
{
    setTrussTarget(.3);
}

void Shooter::cock(int level)
{
    switch (level)
    {
        case HALF_COCK:
            setTarget(5);
            break;
        case FULL_COCK:
            setTarget(11);
            break;
        case NO_COCK:
            setTarget(0);
            winchMotor->Set(0);
            intake->runIntake(0);
            cockTimer->Stop();
            cockTimer->Reset();
            break;
    }

    currPresetName = level;
    cockTimer->Start();
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

void Shooter::killShooter(bool dead)
{
    if (dead)
    {
        cock(NO_COCK);
    }
}

bool Shooter::performFire()
{
    fireTimer->Start();
    intake->setFangs(false);
    intake->runIntake(0);
    winchMotor->Set(0);
    if (fireTimer->Get() >= 0.1)
    {
        winchRelease->Set(true);
    }

    if (fireTimer->Get() >= 0.35)
    {
        fireTimer->Stop();
        fireTimer->Reset();
        intake->runIntake(0);
        firing = false;

        return true;
    }

    return false;
}

bool Shooter::performSoftFire()
{
    fireTimer->Start();
    intake->setFangs(true);
    intake->runIntake(0);
    if (fireTimer->Get() >= 0.25)
    {
        winchRelease->Set(true);
    }
    if (fireTimer->Get() >= 0.5)
    {
        fireTimer->Stop();
        fireTimer->Reset();
        intake->runIntake(0);
        firing = false;

        return true;
    }

    return false;
}

void Shooter::testWinch(float speed)
{
    winchSpeed = speed;
}

void Shooter::update()
{
    //float error = fabs(winchPID->getTarget() - winchDistance());

    // Zero the winch whenever we fire
    currZeroPoint = zeroPoint->Get();
    if ((currZeroPoint) && (!prevZeroPoint)) // This handles actual zeroing
    {
        encoder->Reset();
    }
    else if ((!currZeroPoint) && (prevZeroPoint)) // This is so we actually recock when we want to
    {
        encoder->Reset();
    }

    if (!arm->isCockSafe())
    {
        cock(NO_COCK);
        firing = false;
    }
    else
    {
        if (!arm->isFireAngle())
            firing = false;

        if (firing && arm->isCockSafe())
        {
            if (arm->getPreset() == SHOOTING || arm->getPreset() == HIGH_GOAL)
            {
                if (performFire())
                {
                    //cock(FULL_COCK);
                }
            }
            else if (arm->getPreset() == CLOSE_SHOT || STOW)
            {
                if (performSoftFire())
                {
                    //cock(FULL_COCK);
                }
            }
        }
        else
        {
            // make sure we are cocked
            winchRelease->Set(false);
            if ((!fullCockPoint->Get()) || (winchDistance() >= dangerPoint) || (!arm->isCockSafe()) || (cockTimer->Get() >= 6))
            {
                cock(NO_COCK);
                cockTimer->Stop();
                cockTimer->Reset();
            }
            else if (fullCockPoint->Get() && (winchDistance() < dangerPoint))
            {
                if (winchDistance() < winchPID->getTarget()) // We don't hit the actual distance perfectly and we prefer to be more then less
                {
                    winchMotor->Set(winchPID->update(winchDistance()));
                    intake->runIntake(0.8);
                }
                else
                {
                    winchMotor->Set(0);
                    intake->runIntake(0);
                }
            }
        }
    }

    /*
    SmartDashboard::PutNumber("Truss Winch Error: ", trussPID->getTarget() - Pot->GetVoltage());
    SmartDashboard::PutNumber("Truss PID Output: ", trussPID->update(Pot->GetVoltage()));
    */

    float trussWinchInput = trussPID->update(Pot->GetVoltage());
    if (Pot->GetVoltage() < 4 && Pot->GetVoltage() > 0.3)
    {
        if (fabs(trussWinchInput) > 0.05)
            trussWinch->Set(trussWinchInput);
        else
            trussWinch->Set(0);
    }
    else
        trussWinch->Set(0);

    prevZeroPoint = currZeroPoint;

}

void Shooter::dashboardUpdate()
{
    //SmartDashboard::PutBoolean("Full Cock Point: ", fullCockPoint->Get());
    //SmartDashboard::PutBoolean("Zero Point: ", zeroPoint->Get());
}
