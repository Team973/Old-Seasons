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
}

bool WhipCommand::taskPeriodic() {
    if (timer->Get() >= timeout) {
        return true;
    }

    return false;
}

}
