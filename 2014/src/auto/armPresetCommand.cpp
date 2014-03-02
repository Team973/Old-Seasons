#include "WPILib.h"
#include "armPresetCommand.hpp"

ArmPresetCommand::ArmPresetCommand(Arm *arm_, int preset_)
{
    arm = arm_;
    preset = preset_;
}

void ArmPresetCommand::Init()
{
    arm->setPreset(preset);
}

bool ArmPresetCommand::Run()
{
    if (arm->getError() <= 1)
    {
        return true;
    }
    return false;
}
