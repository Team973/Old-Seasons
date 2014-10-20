#include "WPILib.h"
#include "arm.hpp"
#include "pid.hpp"
#include <map>
#include "utility.hpp"

Arm::Arm(Talon *motor_, Encoder *sensor_)
{
    motor = motor_;
    sensor = sensor_;

    armPID = new PID(0.1);
    armPID->setBounds(-1,1);
    armPID->start();

    presets["trussShot"] = 33;
    presets["intake"] = 108;
    presets["pseudoIntake"] = 67;
    presets["stow"] = 29;
    presets["closeShot"] = 22;
    presets["fenderShot"] = 8.8;
    presets["none"] = 0;

    currPreset = "none";
}

float Arm::getAngle()
{
    float degreesPerRevolution = 360;
    float gearRatio = 1;
    float ticksPerRevolution = 2500;
    return sensor->Get() / (ticksPerRevolution * gearRatio) * degreesPerRevolution;
}

void Arm::setBehavior(std::string state)
{
    setPreset(state);
}

std::string Arm::getBehavior()
{
    return currPreset;
}

void Arm::setPreset(std::string preset)
{
    if (presets.find(preset) != presets.end())
    {
        float p = presets[preset];
        armPID->setTarget(p);
        currPreset = preset;
    }
    else
    {
        armPID->setTarget(presets[currPreset]);
    }
}

void Arm::update()
{
    if (currPreset == "intake")
        armPID->setGains(0.09);
    else
        armPID->setGains(0.1);

    motor->Set(limit(armPID->update(getAngle())));
}
