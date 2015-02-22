#include "driveCommand.hpp"
#include "../../subsystems/xyManager.hpp"
#include "../../lib/flagAccumulator.hpp"

namespace frc973 {

DriveCommand::DriveCommand(float target_, float timeout_)
{
    xyManager = XYManager::getInstance();

    target = target_;

    cmdSeq = NULL;
    distance = 0.0;

    done = true;

    accumulator = new FlagAccumulator();
    accumulator->setThreshold(3);

    setTimeout(timeout_);
}

DriveCommand::DriveCommand(AutoCommand *cmdSeq_, float distance_, float target_, float timeout_) {
    xyManager = XYManager::getInstance();

    cmdSeq = cmdSeq_;

    distance = distance_;

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
    if (cmdSeq != NULL && (fabs(xyManager->getDistanceFromTarget()) >= distance)) {
        done = cmdSeq->run();
    }

    if ((accumulator->update(xyManager->isMovementDone()) || timer->Get() >= timeout) && done)
    {
        return true;
    }

    return false;
}

}
