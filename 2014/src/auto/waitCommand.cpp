#include "WPILib.h"
#include "waitCommand.hpp"

AutoWaitCommand::AutoWaitCommand(float timeout_)
{
    setTimeout(timeout_);
}

void AutoWaitCommand::Init()
{
    timer->Start();
    timer->Reset();
}

bool AutoWaitCommand::Run()
{
    return timer->Get() > timeout;
}
