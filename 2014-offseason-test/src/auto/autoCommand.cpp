#include "autoCommand.hpp"

namespace frc973 {

// sets the timeout and initializes the timer
AutoCommand::AutoCommand() {
    timer = new Timer();
}

// sets timeout in seconds
void AutoCommand::setTimeout(float timeout_) {
    timeout = timeout_;
}

// initializes the command
// called before run
void AutoCommand::init() {
    timer->Reset();
    timer->Start();
}

// called each cycle
// returns true if the command is complete
bool AutoCommand::run() {

    return true;
}

}
