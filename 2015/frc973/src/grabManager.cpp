#include "WPILib.h"
#include "grabManager.hpp"

namespace frc973 {

GrabManager::GrabManager(Solenoid* grabber_) {
    grabber = grabber_;
}

void GrabManager::drop() {
    grabber->Set(true);
}

void GrabManager::retract() {
    grabber->Set(false);
}

}
