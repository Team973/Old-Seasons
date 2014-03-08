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
    hasBall = false;

    possesionTimer = new Timer();

    clamped = false;
    dropTheBall = false;
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
    dropTheBall = overide;
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

    // If we don't have a ball are actively intaking and can actually intake...
    if ((!hasBall) && ((arm->getPreset() != SHOOTING) && !dropTheBall))
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
                    hasBall = true;
                    arm->setPreset(STOW);
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
        else if ((hasBall) && (shooter->isFiring() || dropTheBall)) // Lets the ball go
        {
            hasBall = false;
            dropTheBall = false;
        }
}

void Intake::dashboardUpdate()
{
    SmartDashboard::PutBoolean("Ball Sensor: ", ballSensor->Get());
    SmartDashboard::PutBoolean("Has Ball: ", hasBall);
}
