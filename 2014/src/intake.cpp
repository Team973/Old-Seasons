#include "WPILib.h"
#include "intake.hpp"

Intake::Intake(Arm *arm_, Victor *linearMotor_, Victor *crossMotor_, Solenoid *openClaw_, DigitalInput *ballSensor_)
{
    arm = arm_;
    linearMotor = linearMotor_;
    crossMotor = crossMotor_;
    openClaw = openClaw_;
    ballSensor = ballSensor_;

    intakeManualSpeed = 0;
    hasBall = false;

    possesionTimer = new Timer();
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

void Intake::setFangs(bool state)
{
    openClaw->Set(state);
}

void Intake::update()
{
    float intakeTime = 1;
    //TODO(oliver): Switch test1 to the actual approved intaking preset
    if ((!hasBall) && (intakeSpeed > 0) && (arm->getPreset() == TEST1))
    {
        if (ballSensor)
        {
            if (possesionTimer->Get() == 0)
            {
                possesionTimer->Start();
            }
            else if (possesionTimer->Get() >= intakeTime)
            {
                possesionTimer->Stop();
                hasBall = true;
                arm->setPreset(TEST2);
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
    else if (hasBall) //TODO(oliver): Add the fired boolean whenever that is added to the shooter
    {
        hasBall = false;
    }
    else if (intakeManualSpeed != 0)
    {
        linearMotor->Set(intakeManualSpeed);
        crossMotor->Set(intakeManualSpeed);
    }
    else
    {
        linearMotor->Set(intakeSpeed);
        crossMotor->Set(intakeSpeed);
    }
}

void Intake::dashboardUpdate()
{
}
