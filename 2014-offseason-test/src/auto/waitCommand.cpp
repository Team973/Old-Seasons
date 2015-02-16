#include "waitCommand.hpp"
#include "WPILib.h"

namespace frc973 {

WaitCommand::WaitCommand(float timeout_) {
    setTimeout(timeout_);
}

void WaitCommand::init() {
    timer->Start();
    timer->Reset();
}

bool WaitCommand::run() {
    if (timer->Get() >= timeout) {
        return true;
    }

    return false;
}

}
