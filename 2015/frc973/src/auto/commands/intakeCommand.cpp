#include "WPILib.h"
#include "intakeCommand.hpp"
#include "../../stateManager.hpp"

namespace frc973 {

IntakeCommand::IntakeCommand(StateManager *manager_, float speed_, bool open_, float timeout_) {
    manager = manager_;
    speed = speed_;
    open = open_;
    setTimeout(timeout_);
}

void IntakeCommand::init() {
    timer->Start();
    timer->Reset();
}

bool IntakeCommand::taskPeriodic() {

    if (timer->Get() >= timeout) {
        manager->setIntakeSpeed(speed);
        manager->setIntakePosition(open);
        return true;
    }

    return false;
}

}
