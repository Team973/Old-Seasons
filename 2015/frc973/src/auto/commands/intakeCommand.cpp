#include "WPILib.h"
#include "intakeCommand.hpp"
#include "../../stateManager.hpp"

namespace frc973 {

IntakeCommand::IntakeCommand(StateManager *manager_, float speed_, bool open_, float timeout_) {
    manager = manager_;
    leftSpeed = speed_;
    rightSpeed = speed_;
    open = open_;
    setTimeout(timeout_);
}

IntakeCommand::IntakeCommand(StateManager *manager_, float leftSpeed_, float rightSpeed_, bool open_, float timeout_) {
    manager = manager_;
    leftSpeed = leftSpeed_;
    rightSpeed = rightSpeed_;
    open = open_;
    setTimeout(timeout_);
}

void IntakeCommand::init() {
    timer->Start();
    timer->Reset();
}

bool IntakeCommand::taskPeriodic() {

    if (timer->Get() >= timeout) {
        manager->setIntakeLeftRight(leftSpeed, rightSpeed);
        manager->setIntakePosition(open);
        return true;
    }

    return false;
}

}
