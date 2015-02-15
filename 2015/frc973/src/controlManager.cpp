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

    stateManager->setIntakeFromControls(controls->getCodriverAxis(1));

    if (controls->getCodriverButton(1)) {
        stateManager->setRobotState(StateManager::LOAD);
    }
    if (controls->getCodriverButton(2)) {
        stateManager->setRobotState(StateManager::IDLE);
    }
}

} /* namespace frc973 */
