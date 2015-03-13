#include "driveCommand.hpp"
#include "../../subsystems/xyManager.hpp"
#include "../../lib/flagAccumulator.hpp"
#include "../../stateManager.hpp"

namespace frc973 {

DriveCommand::DriveCommand(StateManager *manager_, float target_, std::string speed_)
{
    xyManager = XYManager::getInstance();
    speed = speed_;
        
    manager = manager_;

    target = target_;

    cmdSeq = NULL;
    distance = 0.0;

    done = true;

    accumulator = new FlagAccumulator();
    accumulator->setThreshold(3);
}

DriveCommand::DriveCommand(StateManager *manager_, AutoCommand *cmdSeq_, float distance_, float target_, std::string speed_) {
    xyManager = XYManager::getInstance();
    speed = speed_;

    manager = manager_;

    cmdSeq = cmdSeq_;

    distance = distance_;

    target = target_;

    accumulator = new FlagAccumulator();
    accumulator->setThreshold(3);
}

void DriveCommand::init()
{
    xyManager->setTargetDistance(target);
    xyManager->setSpeed(speed);
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
        if ((accumulator->update(xyManager->isMovementDone())))
        {
            return true;
        }
    } else {
        xyManager->pauseProfile();
    }

    return false;
}

}
