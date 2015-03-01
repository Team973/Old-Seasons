#include "driveCommand.hpp"
#include "../../subsystems/xyManager.hpp"
#include "../../lib/flagAccumulator.hpp"
#include "../../stateManager.hpp"

namespace frc973 {

DriveCommand::DriveCommand(StateManager *manager_, float target_, bool fast_, float timeout_)
{
    xyManager = XYManager::getInstance();
    xyManager->setSpeed(fast_);
        
    manager = manager_;

    target = target_;

    cmdSeq = NULL;
    distance = 0.0;

    done = true;

    accumulator = new FlagAccumulator();
    accumulator->setThreshold(3);

    setTimeout(timeout_);
}

DriveCommand::DriveCommand(StateManager *manager_, AutoCommand *cmdSeq_, float distance_, float target_, bool fast_, float timeout_) {
    xyManager = XYManager::getInstance();

    manager = manager_;

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
    xyManager->resetProfile();
    xyManager->startProfile();
}

bool DriveCommand::taskPeriodic()
{
    if (cmdSeq != NULL && xyManager->getDistanceFromTarget() >= distance) {
        done = cmdSeq->run();
    }

    if (!manager->isDriveLocked()) {
        timer->Start();
        xyManager->startProfile();
        if ((accumulator->update(xyManager->isMovementDone()) || timer->Get() >= timeout))
        {
            return true;
        }
    } else {
        timer->Stop();
        xyManager->pauseProfile();
    }

    return false;
}

}
