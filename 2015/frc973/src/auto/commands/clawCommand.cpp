#include "clawCommand.hpp"
#include "../../stateManager.hpp"

namespace frc973 {

ClawCommand::ClawCommand(StateManager *manager_, bool open_) {
    manager = manager_;
    open = open_;
}

void ClawCommand::init() {
    manager->actuateClaw(open);
}

bool ClawCommand::taskPeriodic() {
    return true;
}

}
