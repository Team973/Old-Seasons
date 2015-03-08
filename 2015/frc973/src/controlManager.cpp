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
    stateManager->setDriveFromControls(controls->getThrottle(), -controls->getTurn(), controls->getDriverButton(8));
    stateManager->vTecKickedInYo(controls->getDriverButton(6));

    stateManager->setIntakeSpeed(-controls->getCodriverAxis(1));

    stateManager->setElevatorFromControls(controls->getCodriverAxis(3));

    if (controls->getCodriverButton(1)) {
        stateManager->setAutoLoadReady();
    }

    if (controls->getCodriverButton(2)) {
    }

    if (controls->getCodriverButton(3)) {
        stateManager->setLoadReady();
    }

    if (controls->getCodriverButton(4)) {
        stateManager->setRestingLoad();
    }

    stateManager->setLastTote(controls->getCodriverButton(5));
    stateManager->setContainerPickup(controls->getCodriverButton(6));

    stateManager->setIntakePosition(controls->getCodriverButton(8));

    if (controls->getCodriverDpadUp()) {
        stateManager->incrementElevatorHeight(5);
    }

    if (controls->getCodriverDpadDown()) {
        stateManager->incrementElevatorHeight(-5);
    }
}

} /* namespace frc973 */
