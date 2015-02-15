#include "WPILib.h"
#include "corralCommand.hpp"
#include "../intake.hpp"

CorralCommand::CorralCommand(Intake *intake_, bool trap_)
{
    intake = intake_;
    trap = trap_;
}

void CorralCommand::Init()
{
    intake->dropCorral(trap);
}

bool CorralCommand::Run()
{
    return true;
}
