#include "WPILib.h"
#include "fireCommand.hpp"

FireCommand::FireCommand(Shooter *shooter_)
{
    shooter = shooter_;
}

void FireCommand::Init()
{
    shooter->fire(true);
}

bool FireCommand::Run()
{
    return true;
}
