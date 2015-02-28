#include "WPILib.h"
#include "sauropodPathCommand.hpp"
#include "../../stateManager.hpp"
#include "../../subsystems/sauropod.hpp"

namespace frc973 {

SauropodPathCommand::SauropodPathCommand(StateManager *manager_, int path_, float timeout_) {
    manager = manager_;
    path = path_;
    setTimeout(timeout_);
}

void SauropodPathCommand::init() {
    timer->Start();
    timer->Reset();
    if (path == Sauropod::READY) {
        manager->setAutoLoadReady();
    } else {
        manager->setSauropodPath(path);
    }
}

bool SauropodPathCommand::taskPeriodic() {
    if (manager->isSauropodDone() || timer->Get() > timeout) {
        return true;
    }

    return false;
}

}
