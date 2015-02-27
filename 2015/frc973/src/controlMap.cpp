#include "WPILib.h"
#include "controlMap.hpp"

namespace frc973 {

ControlMap::ControlMap(Joystick *driver_, Joystick *coDriver_) {
    coDriver = coDriver_;
    driver = driver_;

    prevCodriverDpad = 0.0;
}

float ControlMap::getThrottle() {
    return driver->GetRawAxis(1);
}

float ControlMap::getTurn() {
    return driver->GetRawAxis(2);
}

bool ControlMap::getLowGear() {
    return driver->GetRawButton(5);
}

bool ControlMap::getCodriverButton(int button) {
    return coDriver->GetRawButton(button);
}

float ControlMap::getCodriverAxis(int axis) {
    return coDriver->GetRawAxis(axis);
}

bool ControlMap::getDriverButton(int button) {
    return driver->GetRawButton(button);
}

float ControlMap::getDriverAxis(int axis) {
    return driver->GetRawAxis(axis);
}

bool ControlMap::getCodriverDpadDown() {
    double axis = coDriver->GetRawAxis(6);
    bool down = axis > 0.5 && prevCodriverDpad <= 0.5;
    prevCodriverDpad = axis;
    return down;
}

bool ControlMap::getCodriverDpadUp() {
    double axis = coDriver->GetRawAxis(6);
    bool up = axis < -0.5 && prevCodriverDpad >= -0.5;
    prevCodriverDpad = axis;
    return up;
}

}
