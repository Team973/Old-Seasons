#include "driveCommand.hpp"
#include "../../subsystems/xyManager.hpp"
#include "../../lib/flagAccumulator.hpp"

namespace frc973 {

DriveCommand::DriveCommand(float target_, float timeout_)
{
    xyManager = XYManager::getInstance();

    target = target_;

    cmdSeq = NULL;
    percent = 0.0;

    done = true;

    accumulator = new FlagAccumulator();
    accumulator->setThreshold(3);

    setTimeout(timeout_);
}

DriveCommand::DriveCommand(AutoCommand *cmdSeq_, float percent_, float target_, float timeout_) {
    xyManager = XYManager::getInstance();

    cmdSeq = cmdSeq_;

    percent = percent_;

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
    if (cmdSeq != NULL && (xyManager->getDistanceFromTarget() >= (xyManager->getDistanceFromTarget()*(percent/10)))) {
        done = cmdSeq->run();
    }

    if ((accumulator->update(xyManager->isMovementDone()) || timer->Get() >= timeout) && done)
    {
        return true;
    }

    return false;
}

}
