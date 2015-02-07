#include "WPILib.h"
#include "stateManager.hpp"
#include "lib/utility.hpp"
#include "subsystems/drive.hpp"
#include "subsystems/sauropod.hpp"

namespace frc973 {

StateManager::StateManager(Drive *drive_, Sauropod *sauropod_) {
    drive = drive_;
    sauropod = sauropod_;
}

void StateManager::setDriveFromControls(double throttle, double turn, bool quickTurn) {
    drive->CheesyDrive(deadband(throttle, 0.1), -deadband(turn, 0.1), false, quickTurn);
}

void StateManager::setElevatorPreset(std::string preset) {
    sauropod->setPreset(preset);
}

void StateManager::update() {
    sauropod->update();
}

}