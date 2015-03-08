#include "WPILib.h"
#include "sauropodCommand.hpp"
#include "../../stateManager.hpp"

namespace frc973 {

SauropodCommand::SauropodCommand(StateManager *manager_, std::string preset_, float toteTimeout_, float timeout_) {
    manager = manager_;
    preset = preset_;
    toteTimeout = toteTimeout_;
    setTimeout(timeout_);

    moving = false;
}

void SauropodCommand::init() {
    timer->Start();
    timer->Reset();
}

bool SauropodCommand::taskPeriodic() {
    if (manager->gotTote() || timer->Get() > toteTimeout) {
        manager->setSauropodPreset(preset);
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
