#include "WPILib.h"
#include "corralCommand.hpp"
#include "../intake.hpp"

CorralCommand::CorralCommand(Intake *intake_)
{
    intake = intake_;
}

void CorralCommand::Init()
{
}

bool CorralCommand::Run()
{
    return true;
}
