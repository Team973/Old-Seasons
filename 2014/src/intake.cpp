#include "WPILib.h"
#include "arm.hpp"
#include "shooter.hpp"
#include "intake.hpp"

Intake::Intake(Victor *linearMotor_, Victor *crossMotor_, Solenoid *openClaw_, Solenoid *corral_, DigitalInput *ballSensor_)
{
    linearMotor = linearMotor_;
    crossMotor = crossMotor_;
    openClaw = openClaw_;
    corral = corral_;
    ballSensor = ballSensor_;

    intakeManualSpeed = 0;

    possesionTimer = new Timer();

    clamped = false;
    hasBall = false;
}

void Intake::initialize(Arm *arm_, Shooter *shooter_)
{
    arm = arm_;
    shooter = shooter_;
}

float Intake::limit(float x)
{
    if (x > 1)
        return 1;
    else if (x < -1)
        return -1;
    else
        return x;
}

void Intake::manual(float speed)
{
    intakeManualSpeed = speed;
}

void Intake::runIntake(float speed)
{
    intakeSpeed = speed;
}

void Intake::setFangs(bool state, bool overide)
{
    openClaw->Set(state);
    clamped = state;
}

bool Intake::isClamped()
{
    return clamped;
}

void Intake::stop()
{
    linearMotor->Set(0);
    crossMotor->Set(0);
}

bool Intake::gotBall()
{
    return hasBall;
}

void Intake::update()
{
    float intakeTime = 0.25;
    // Manual overrides everything
    if (intakeManualSpeed != 0)
    {
        linearMotor->Set(intakeManualSpeed);
        crossMotor->Set(intakeManualSpeed);
    }
    else
    {
        linearMotor->Set(intakeSpeed);
        crossMotor->Set(intakeSpeed);
    }

    if (hasBall && shooter->isFiring())
    {
        hasBall = false;
    }
    // If we don't have a ball are actively intaking and can actually intake...
    else if (arm->getPreset() == (INTAKE || PSEUDO_INTAKE))
        {
            if (!ballSensor->Get())
            {
                if (possesionTimer->Get() == 0)
                {
                    possesionTimer->Start();
                }
                else if (possesionTimer->Get() >= intakeTime)
                {
                    possesionTimer->Stop();
                    arm->setPreset(STOW);
                    hasBall = true;
                }

            }
            else
            {
                if (possesionTimer->Get() > 0)
                {
                    possesionTimer->Stop();
                    possesionTimer->Reset();
                }
            }
        }
}

void Intake::dashboardUpdate()
{
}
