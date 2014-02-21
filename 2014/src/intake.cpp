#include "WPILib.h"
#include "arm.hpp"
#include "shooter.hpp"
#include "intake.hpp"

Intake::Intake(Arm *arm_, Shooter *shooter_, Victor *linearMotor_, Victor *crossMotor_, Solenoid *openClaw_, Solenoid *corral_, DigitalInput *ballSensor_)
{
    arm = arm_;
    shooter = shooter_;
    linearMotor = linearMotor_;
    crossMotor = crossMotor_;
    openClaw = openClaw_;
    corral = corral_;
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
    float intakeTime = 0.5;
    // Manual overrides everything
    if (intakeManualSpeed != 0)
    {
        linearMotor->Set(intakeManualSpeed);
        crossMotor->Set(intakeManualSpeed);
    }
    else
    {
        // If we don't have a ball are actively intaking and can actually intake...
        if ((!hasBall) && (intakeSpeed > 0) && ((arm->getPreset() == INTAKE)))
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
                    openClaw->Set(false);
                    arm->setPreset(SHOOTING);
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
        else if ((hasBall) && (shooter->isFiring())) // Lets the ball go
        {
            hasBall = false;
        }

        linearMotor->Set(intakeSpeed);
        crossMotor->Set(intakeSpeed);
    }

}

void Intake::dashboardUpdate()
{
    SmartDashboard::PutBoolean("Ball Sensor: ", ballSensor->Get());
}
