#include "WPILib.h"
#include "controlManager.hpp"
#include "controlMap.hpp"
#include "stateManager.hpp"
#include "subsystems/sauropod.hpp"

namespace frc973 {

ControlManager::ControlManager(ControlMap *controls_, StateManager *stateManager_) {
    controls = controls_;
    stateManager = stateManager_;
}

void ControlManager::update() {
    stateManager->setDriveFromControls(controls->getThrottle(), controls->getTurn(), controls->getQuickturn());

    stateManager->setIntakeSpeed(controls->getCodriverAxis(1));

    if (controls->getCodriverButton(1)) {
        stateManager->setSauropodPath(Sauropod::READY);
    }
    if (controls->getCodriverButton(2)) {
        stateManager->setSauropodPath(Sauropod::IDLE);
    }
    if (controls->getCodriverButton(3)) {
        stateManager->setSauropodPath(Sauropod::PICKUP);
    }
    if (controls->getCodriverButton(4)) {
        stateManager->setSauropodPath(Sauropod::RESTING);
    }
    if (controls->getCodriverButton(5)) {
        stateManager->setSauropodPath(Sauropod::PLATFORM);
    }
    if (controls->getCodriverButton(6)) {
        stateManager->setSauropodPath(Sauropod::RETURN);
    }
    if (controls->getCodriverButton(7)) {
        stateManager->setWhipPosition("extend");
    }
    if (controls->getCodriverButton(8)) {
        stateManager->setWhipPosition("retract");
    }
}

} /* namespace frc973 */
