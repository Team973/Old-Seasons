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
    firingIndexer = false;
    discsFired = 0;
    targetFlywheelSpeed = 5500;
}

// The following computes and controls the speed of the flywheel
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
    firingIndexer = t;
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

void Shooter::clearDiscsFired()
{
    discsFired = 0;
}

void Shooter::autoFire()
{
    setFlywheelRunning(true);

    if (discsFired < 4)
    {
        if (getFlywheelSpeed() >= targetFlywheelSpeed)
        {
            indexer->Set(true);
            discsFired++;
        }
        else
            indexer->Set(false);
    }
    else
    {
        setFlywheelRunning(false);
        indexer->Set(false);
    }

}

void Shooter::update()
{
    if (flywheelRunning)
    {
        flywheelMotor->Set(RPMcontrol(getFlywheelSpeed()));
        indexer->Set(firingIndexer);
    }
    else
    {
        flywheelMotor->Set(0.0);
        indexer->Set(false);
    }

    if (rollerRunning)
    {
        if ((indexer->Get()) && (shotAngle->Get() == true))
            roller->Set(.2);
        else
            roller->Set(.8);
    }
    else
    {
        roller->Set(.0);
    }
}
