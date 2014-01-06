#include "WPILib.h"
#include "autoCommand.hpp"

AutoCommand::AutoCommand()
{
    timer = new Timer();
}

void AutoCommand::setTimeout(double timeout_)
{
    timeout = timeout_;
}

void AutoCommand::Init()
{
    timer->Start();
    timer->Reset();
}

bool AutoCommand::Run()
{
    return true;
}
