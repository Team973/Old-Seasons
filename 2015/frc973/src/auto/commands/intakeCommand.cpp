#include "WPILib.h"
#include "intakeCommand.hpp"
#include "../../stateManager.hpp"

namespace frc973 {

IntakeCommand::IntakeCommand(StateManager *manager_, float speed_, bool position_, float timeout_) {
    manager = manager_;
    leftSpeed = speed_;
    rightSpeed = speed_;
    position = position_;
    setTimeout(timeout_);
}

IntakeCommand::IntakeCommand(StateManager *manager_, float leftSpeed_, float rightSpeed_, bool position_, float timeout_) {
    manager = manager_;
    leftSpeed = leftSpeed_;
    rightSpeed = rightSpeed_;
    position = position_;
    setTimeout(timeout_);
}

void IntakeCommand::init() {
    timer->Start();
    timer->Reset();
}

bool IntakeCommand::taskPeriodic() {

    if (timer->Get() >= timeout) {
        manager->setIntakeLeftRight(leftSpeed, rightSpeed);
        manager->setIntakePosition(position);
        return true;
    }

    return false;
}

}
