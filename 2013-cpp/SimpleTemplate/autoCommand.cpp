#include "autoCommand.hpp"

AutoCommand::AutoCommand()
{
    timer = new Timer();
}

void AutoCommand::setTimeout(double timeout_)
{
    timeout = timeout_;
}

bool AutoCommand::Init()
{
    timer->Start();
    timer->Reset();
    return true;
}

bool AutoCommand::Run()
{
    return true;
}
