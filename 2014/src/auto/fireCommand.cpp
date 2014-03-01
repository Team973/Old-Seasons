#include "WPILib.h"
#include "fireCommand.hpp"

FireCommand::FireCommand(Shooter *shooter_, float timeout_)
{
    setTimeout(timeout_);

    shooter = shooter_;
}

void FireCommand::Init()
{
    timer->Start();
    timer->Reset();
}

bool FireCommand::Run()
{
    return true;
}
