#include "turnCommand.hpp"
#include "../../subsystems/xyManager.hpp"
#include "../../stateManager.hpp"

namespace frc973 {

TurnCommand::TurnCommand(StateManager *manager_, float target_, float speed_, float timeout_)
{
    xyManager = XYManager::getInstance();

    manager = manager_;

    target = target_;
    speed = speed_;

    setTimeout(timeout_);
}

void TurnCommand::init()
{
    timer->Start();
    timer->Reset();
    xyManager->setTargetAngle(target);
    xyManager->limitTurnSpeed(speed);
    xyManager->resetProfile();
    xyManager->startProfile();
}

bool TurnCommand::taskPeriodic()
{
    if (!manager->isDriveLocked()) {
        timer->Start();
        xyManager->startProfile();
        if (xyManager->isMovementDone() || timer->Get() >= timeout)
        {
            return true;
        }
    } else {
        timer->Reset();
        xyManager->pauseProfile();
    }

    return false;
}

}
