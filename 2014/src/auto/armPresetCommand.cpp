#include "WPILib.h"
#include "armPresetCommand.hpp"

ArmPresetCommand::ArmPresetCommand(Arm *arm_, int preset_, float timeout_)
{
    arm = arm_;
    preset = preset_;
    setTimeout(timeout_);
}

void ArmPresetCommand::Init()
{
    timer->Start();
    timer->Reset();
    arm->setPreset(preset);
}

bool ArmPresetCommand::Run()
{
    if (arm->getError() <= 1 || timer->Get() >= timeout)
    {
        return true;
    }
    return false;
}
