#include "WPILib.h"
#include "switchCommand.hpp"

SwitchCommand::SwitchCommand()
{
}

void SwitchCommand::Init()
{
    timer->Start();
    timer->Reset();
}

bool SwitchCommand::Run()
{
    return true;
}
