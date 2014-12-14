#include "waitCommand.hpp"
#include "WPILib.h"

Wait::Wait(float timeout_) {
    setTimeout(timeout_);
}

void Wait::init() {
    timer->Start();
    timer->Reset();
}

bool Wait::run() {
    if (timer->Get() >= timeout) {
        return true;
    }

    return false;
}
