#include "arm.hpp"
#include "pid.hpp"
#include <map>
#include "utility.hpp"
#include "constants.hpp"

Arm::Arm(Talon *motor_, Encoder *sensor_, AnalogChannel *pot_)
{
    motor = motor_;
    sensor = sensor_;
    pot = pot_;

    potZero = pot->GetVoltage();

    printf("constant 'armKP' has the value %f in subsystem 'arm'\n", Constants::getConstant("armKP")->getDouble());

    armPID = new PID(Constants::getConstant("armKP")->getDouble(), Constants::getConstant("armKI")->getDouble(), Constants::getConstant("armKD")->getDouble());
    armPID->setBounds(-1,1);
    armPID->start();

    presets["trussShot"] = Constants::getConstant("trussShot")->getDouble();
    presets["intake"] = Constants::getConstant("intake")->getDouble();
    presets["pseudoIntake"] = Constants::getConstant("pseudoIntake")->getDouble();
    presets["stow"] = Constants::getConstant("stow")->getDouble();
    presets["none"] = 0;

    currPreset = "none";
}

float Arm::getAngle()
{
    float multiplier = (112.01 - 0.00)/(1.87 - 5.01);
    return (pot->GetVoltage() - potZero) * multiplier;
}


void Arm::setBehavior(std::string state)
{
    setPreset(state);
}

std::string Arm::getBehavior()
{
    return currPreset;
}

bool Arm::isShotSafe()
{
    if (getAngle() > 11)
        return true;

    return false;
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
    motor->Set(limit(armPID->update(getAngle())));
}
