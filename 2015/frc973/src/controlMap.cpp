#include "WPILib.h"
#include "controlMap.hpp"

namespace frc973 {

ControlMap::ControlMap(Joystick *driver_, Joystick *coDriver_) {
    coDriver = coDriver_;
    driver = driver_;
}

float ControlMap::getThrottle() {
    return driver->GetRawAxis(1);
}

float ControlMap::getTurn() {
    return driver->GetRawAxis(2);
}

bool ControlMap::getQuickturn() {
    return driver->GetRawButton(5);
}

bool ControlMap::getCodriverButton(int button) {
    return coDriver->GetRawButton(button);
}

}
