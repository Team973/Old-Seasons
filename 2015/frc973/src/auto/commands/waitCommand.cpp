#include "waitCommand.hpp"

namespace frc973 {

WaitCommand::WaitCommand(float timeout_)
{
    setTimeout(timeout_);
}

void WaitCommand::init()
{
    timer->Reset();
    timer->Start();
}

bool WaitCommand::taskPeriodic()
{
    return timer->Get() >= timeout;
}

}
