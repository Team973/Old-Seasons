#include "timerDriveCommand.hpp"
#include "../../stateManager.hpp"

namespace frc973 {

TimerDriveCommand::TimerDriveCommand(StateManager *manager_, int direction_, float time_) {
    manager = manager_;
    direction = -direction_;
    setTimeout(time_);
}

void TimerDriveCommand::init() {
    timer->Reset();
    timer->Start();
}

bool TimerDriveCommand::taskPeriodic() {
    manager->setDriveFromControls(1.0 * direction, 0.0, false);
    return timer->Get() >= timeout;
}

}
