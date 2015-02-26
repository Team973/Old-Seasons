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
    stateManager->setDriveFromControls(controls->getThrottle(), -controls->getTurn(), controls->getLowGear());
    stateManager->vTecKickedInYo(controls->getDriverButton(6));

    stateManager->setIntakeSpeed(-controls->getCodriverAxis(1));

    if (controls->getCodriverButton(1)) {
        stateManager->setAutoLoadReady();
    }

    if (controls->getCodriverButton(2)) {
        stateManager->setContainerPickup();
    }

    if (controls->getCodriverButton(3)) {
        stateManager->setLoadReady();
    }

    if (controls->getCodriverButton(4)) {
        stateManager->setRestingLoad();
    }

    stateManager->setLastTote(controls->getCodriverButton(5));

    stateManager->setCap(controls->getCodriverButton(6));

    stateManager->dropHumanIntake(controls->getCodriverButton(7));

    stateManager->setIntakePosition(controls->getCodriverButton(8));
}

} /* namespace frc973 */
