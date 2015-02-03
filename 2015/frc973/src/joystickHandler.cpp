#include "WPILib.h"
#include "joystickHandler.hpp"

namespace frc973 {

JoystickManager::JoystickManager(Joystick *coDriver_, Joystick *driver_) {
    coDriver = coDriver_;
    driver = driver_;
}

float JoystickManager::getThrottle() {
    return driver->GetRawAxis(1);
}

float JoystickManager::getTurn() {
    return driver->GetRawAxis(4);
}

}
