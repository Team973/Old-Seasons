#include "autoCommand.hpp"

namespace frc973 {

// sets the timeout and initializes the timer
AutoCommand::AutoCommand() {
    timer = new Timer();

    initYet = false;
    doneYet = false;
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

/**
 * Called repeatedly to do the work of the task.
 * Should be redefined by any childrens classes.
 * Returns true to signal that the task is done.
 */
bool AutoCommand::taskPeriodic() {
    return true;
}

/**
 * Initialize task if it has not been initialized,
 * then call taskPeriodic and return the result
 */
bool AutoCommand::run() {
    if (!initYet) {
        initYet = true;
        init();
    }
    if (!doneYet) {
        doneYet = taskPeriodic();
    }
    return doneYet;
}

}
