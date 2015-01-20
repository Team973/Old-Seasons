#include "driveCommand.hpp"
#include "../subsystems/xyManager.hpp"

namespace frc973 {

DriveCommand::DriveCommand(float target_, float timeout_)
{
    xyManager = XYManager::getInstance();
    xyManager->setTargetDistance(target_);

    setTimeout(timeout_);
}

void DriveCommand::init()
{
    timer->Start();
    timer->Reset();
    xyManager->startProfile();
}

bool DriveCommand::taskPeriodic()
{
    if (xyManager->isMovementDone() || timer->Get() >= timeout)
    {
        return true;
    }

    return false;
}

}
