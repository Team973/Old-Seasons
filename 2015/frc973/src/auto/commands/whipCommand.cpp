#include "WPILib.h"
#include "whipCommand.hpp"
#include "../../stateManager.hpp"

namespace frc973 {

WhipCommand::WhipCommand(StateManager *manager_, std::string position_, float timeout_) {
    manager = manager_;
    position = position_;
    setTimeout(timeout_);
}

void WhipCommand::init() {
    timer->Start();
    timer->Reset();
    manager->setWhipPosition(position);
    manager->lockDrive();
}

bool WhipCommand::taskPeriodic() {
    if (!manager->isWhipLocked()) {
        if (manager->isWhipDone() || timer->Get() >= timeout) {
            manager->unlockDrive();
            return true;
        }
    }

    return false;
}

}
