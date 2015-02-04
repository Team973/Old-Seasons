#include "WPILib.h"
#include "stateManager.hpp"
#include "lib/utility.hpp"
#include "subsystems/drive.hpp"

namespace frc973 {

StateManager::StateManager(Drive *drive_) {
    drive = drive_;
}

void StateManager::update() {
    /*
    drive->CheesyDrive(deadband(controls->getThrottle(), 0.1), -deadband(controls->getTurn(), 0.1), false, controls->getQuickturn());

    drive->update();
    */
}

}
