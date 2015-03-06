#include "WPILib.h"
#include "containerGrabber.hpp"

namespace frc973 {

ContainerGrabber::ContainerGrabber(Solenoid* solenoid_) {
    
    solenoid = solenoid_;
}

void ContainerGrabber::grab() {
    solenoid->Set(true);
}

void ContainerGrabber::retract() {
    solenoid->Set(false);
}

} /* namespace frc973 */
