#include "turnCommand.hpp"
#include "../subsystems/xyManager.hpp"

namespace frc973 {

TurnCommand::TurnCommand(float target_, float timeout_)
{
    xyManager = XYManager::getInstance();

    target = target_;

    setTimeout(timeout_);
}

void TurnCommand::init()
{
    timer->Start();
    timer->Reset();
    xyManager->setTargetAngle(target);
    xyManager->startProfile();
}

bool TurnCommand::taskPeriodic()
{
    if (xyManager->isMovementDone() || timer->Get() >= timeout)
    {
        return true;
    }

    return false;
}

}
