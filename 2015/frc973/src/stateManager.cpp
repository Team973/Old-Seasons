#include "WPILib.h"
#include "stateManager.hpp"
#include "lib/utility.hpp"
#include "subsystems/drive.hpp"

namespace frc973 {

StateManager::StateManager(Drive *drive_) {
    drive = drive_;
}

void StateManager::setDriveFromControls(double throttle, double turn, bool quickTurn) {
    drive->CheesyDrive(deadband(throttle, 0.1), -deadband(turn, 0.1), false, quickTurn);
}

void StateManager::update() {
}

}
