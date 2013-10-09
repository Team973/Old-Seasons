#include "WPILib.h"
#include "shooter.hpp"

Shooter::Shooter()
{
    roller = new Talon(6);
    flywheelMotor = new Talon(3);
    indexer = new Solenoid(1);
    flywheelCounter = new Counter(1);
    flywheelCounter->Start();
    shotAngle = new Solenoid(4);
    rollerRunning = false;
    flywheelRunning = false;
}

float Shooter::computeFlywheelSpeed(float counter)
{
    const int flywheelTicksPerRevolution = 1;
    return 60 / (flywheelCounter->GetPeriod() * flywheelTicksPerRevolution);
}

float Shooter::getFlywheelSpeed()
{
    return computeFlywheelSpeed(flywheelCounter->GetPeriod());
}

// Bang-bang control
float Shooter::RPMcontrol(float rpm)
{
    const int dangerRPM = 10000;
    const int targetFlywheelRPM = 5500;

    if (rpm > dangerRPM)
        return 0;
    else if (rpm < targetFlywheelRPM)
        return 1;
    else
        return .4;
}

void Shooter::setIndexer(bool t)
{
    indexer->Set(t);
}

void Shooter::setShotAngle(bool h)
{
    shotAngle->Set(h);
}

void Shooter::setRollerRunning(bool r)
{
    rollerRunning = r;
}

void Shooter::setFlywheelRunning(bool f)
{
    flywheelRunning = f;
}

void Shooter::update()
{
    if (flywheelRunning)
    {
        flywheelMotor->Set(RPMcontrol(getFlywheelSpeed()));
    }
    else
    {
        flywheelMotor->Set(0.0);
        indexer->Set(false);
    }

    if (rollerRunning && !shotAngle)
    {
        roller->Set(.2);
    }
    else
    {
        if (rollerRunning)
        {
            roller->Set(.8);
        }
        else
        {
            roller->Set(.0);
        }
    }
}
