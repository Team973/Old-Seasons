#include "grabCommand.hpp"
#include "../../grabManager.hpp"

namespace frc973 {

GrabCommand::GrabCommand(GrabManager* grabber_, bool retract_) {
    grabber = grabber_;
    retract = retract_;
}

void GrabCommand::init() {
    retract ? grabber->retract() : grabber->drop();
}

bool GrabCommand::taskPeriodic() {
    return true;
}

}
