#include "shooter.hpp"
#include "WPILib.h"
#include "pid.hpp"
#include "utility.hpp"
#include <map>

namespace frc973 {

Shooter::Shooter(Victor *winchMotor_, Solenoid *winchRelease_, DigitalInput *fullCockPoint_)
{
    winchMotor = winchMotor_;
    winchMotor->Set(0);
    winchRelease = winchRelease_;
    fullCockPoint = fullCockPoint_;

    winchPID = new PID(100000);
    winchPID->setBounds(0,1);
    winchPID->start();

    fireTimer = new Timer();
    cockTimer = new Timer();

    firing = false;

    presets["fullCock"] = 11;
    presets["noCock"] = 0;
}

void Shooter::setTarget(float target)
{
    winchPID->setTarget(target);
}

void Shooter::cock(std::string preset)
{
    if (presets.find(preset) != presets.end())
    {
        float p = presets[preset];
        winchPID->setTarget(p);
        currPreset = preset;
    }
    else
    {
        winchPID->setTarget(presets[currPreset]);
    }

    cockTimer->Reset();
}

void Shooter::wantFire()
{
    firing = true;
    fireTimer->Start();
}

void Shooter::setBehavior(std::string preset)
{
    cock(preset);
}

bool Shooter::isFiring()
{
    return firing;
}

void Shooter::update()
{
    if (firing)
    {
        winchRelease->Set(true);
        if (fireTimer->Get() >= 0.5)
        {
            fireTimer->Stop();
            fireTimer->Reset();
            firing = false;
        }
    }
    else if (!fullCockPoint->Get() || cockTimer->Get() >= 6)
    {
        cock("noCock");
        winchMotor->Set(0);
        cockTimer->Stop();
        cockTimer->Reset();
    }
    else
    {
        winchRelease->Set(false);
        winchMotor->Set(limit(winchPID->update(0))); // this means that when we reach our set point the pid automatically turns off
        // not the cleanest thing but I already wrote it and don't feel like cleaning it up
    }
}

}
