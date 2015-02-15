#include "driveCommand.hpp"
#include "../../subsystems/xyManager.hpp"
#include "../../lib/flagAccumulator.hpp"

namespace frc973 {

DriveCommand::DriveCommand(float target_, float timeout_)
{
    xyManager = XYManager::getInstance();

    target = target_;

    accumulator = new FlagAccumulator();
    accumulator->setThreshold(3);

    setTimeout(timeout_);
}

void DriveCommand::init()
{
    timer->Start();
    timer->Reset();
    xyManager->setTargetDistance(target);
    xyManager->startProfile();
}

bool DriveCommand::taskPeriodic()
{
    if (accumulator->update(xyManager->isMovementDone()) || timer->Get() >= timeout)
    {
        return true;
    }

    return false;
}

}
