#include "WPILib.h"
#include "sauropodPathCommand.hpp"
#include "../../stateManager.hpp"
#include "../../subsystems/sauropod.hpp"

namespace frc973 {

SauropodPathCommand::SauropodPathCommand(StateManager *manager_, int path_, float toteTimeout_, float timeout_) {
    manager = manager_;
    path = path_;
    toteTimeout = toteTimeout_;
    setTimeout(timeout_);

    moving = false;
}

void SauropodPathCommand::init() {
    timer->Start();
    timer->Reset();
}

bool SauropodPathCommand::taskPeriodic() {
    if (manager->gotTote() || timer->Get() > toteTimeout) {
        manager->setSauropodPath(path);
        moving = true;
    }

    if (moving) {
        if (manager->isSauropodDone() || timer->Get() > timeout) {
            return true;
        }
    }

    return false;
}

}
