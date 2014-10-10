#include "WPILib.h"
#include "arm.hpp"
#include "pid.hpp"
#include <map>

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

    lastPreset = "none";
}

float Arm::getAngle()
{
    float degreesPerRevolution = 360;
    float gearRatio = 1;
    float ticksPerRevolution = 2500;
    return sensor->Get() / (ticksPerRevolution * gearRatio) * degreesPerRevolution;
}


void Arm::setBehavior(std::string preset)
{
    if (presets.find(preset) != presets.end())
    {
        float p = presets[preset];
        armPID->setTarget(p);
        lastPreset = preset;
    }
    else
    {
        armPID->setTarget(presets[lastPreset]);
    }
}

void Arm::update()
{
    if (lastPreset == "intake")
        armPID->setGains(0.09);

    motor->Set(armPID->update(getAngle()));
}
