#include "WPILib.h"
#include "controlManager.hpp"
#include "controlMap.hpp"
#include "stateManager.hpp"

namespace frc973 {

ControlManager::ControlManager(ControlMap *controls_, StateManager *stateManager_) {
    controls = controls_;
    stateManager = stateManager_;
}

void ControlManager::update() {
    stateManager->setDriveFromControls(controls->getThrottle(), controls->getTurn(), controls->getQuickturn());

    if (controls->getCodriverButton(1)) {
        stateManager->setElevatorPreset("test1");
    }
    if (controls->getCodriverButton(2)) {
        stateManager->setElevatorPreset("test2");
    }
    if (controls->getCodriverButton(3)) {
        stateManager->setElevatorPreset("test3");
    }
}

} /* namespace frc973 */
