#include "WPILib.h"
#include "fireCommand.hpp"

FireCommand::FireCommand(Shooter *shooter_, float timeout_)
{
    shooter = shooter_;
    setTimeout(timeout_);
}

void FireCommand::Init()
{
    timer->Start();
    timer->Reset();
    shooter->fire(true);
}

bool FireCommand::Run()
{
    if (timer->Get() > timeout)
        return true;
    else
        return false;
}
