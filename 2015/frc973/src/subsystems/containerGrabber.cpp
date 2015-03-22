#include "WPILib.h"
#include "containerGrabber.hpp"

namespace frc973 {

ContainerGrabber::ContainerGrabber(Solenoid* solenoid_, CANTalon* motor_) {
    solenoid = solenoid_;
    motor = motor_;
}

void ContainerGrabber::testMotor(float speed) {
    motor->Set(speed);
}

void ContainerGrabber::grab() {
    solenoid->Set(true);
}

void ContainerGrabber::retract() {
    solenoid->Set(false);
}

} /* namespace frc973 */
