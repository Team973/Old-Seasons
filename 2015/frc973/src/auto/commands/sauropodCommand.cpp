#include "WPILib.h"
#include "sauropodCommand.hpp"
#include "../../stateManager.hpp"

namespace frc973 {

SauropodCommand::SauropodCommand(StateManager *manager_, std::string preset_, float toteTimeout_, bool braked_, float timeout_, bool ignoreSensor_) {
    manager = manager_;
    preset = preset_;
    toteTimeout = toteTimeout_;
    braked = braked_;
    setTimeout(timeout_);

    ignoreSensor = ignoreSensor_;

    moving = false;
}

void SauropodCommand::init() {
    timer->Start();
    timer->Reset();
}

bool SauropodCommand::taskPeriodic() {
    if ((manager->gotTote() && !ignoreSensor) || timer->Get() > toteTimeout) {
        manager->setSauropodPreset(preset);
        moving = true;
    }

    if (moving) {
        if (manager->isSauropodDone() || timer->Get() > timeout) {
            if (braked && manager->isSauropodDone()) {
                manager->brakeClaw();
                return true;
            } else if (!braked) {
                return true;
            }
        }
    }

    return false;
}

}
