#include "WPILib.h"
#include "grabCommand.hpp"
#include "../../stateManager.hpp"

namespace frc973 {

GrabCommand::GrabCommand(StateManager *manager_, bool grab_) {
    manager = manager_;
    grab = grab_;
}

void GrabCommand::init() {
    if (grab) {
        manager->dropGrabber();
    } else {
        manager->raiseGrabber();
    }
}

bool GrabCommand::taskPeriodic() {
    return true;
}

}
