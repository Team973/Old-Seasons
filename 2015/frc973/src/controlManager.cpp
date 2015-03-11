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

    stateManager->setAutoLoad(controls->getCodriverButton(1));

    if (controls->getCodriverButton(2)) {
        stateManager->setScore();
    }

    stateManager->setHumanLoad(controls->getCodriverButton(3));

    if (controls->getCodriverButton(4)) {
        stateManager->setResting();
    }

    if (controls->getCodriverButton(5)) {
        stateManager->setLastTote();
    }

    stateManager->setContainerLoad(controls->getCodriverButton(6));

    stateManager->actuateClaw(!controls->getCodriverButton(7));

    if (controls->getCodriverButton(8)) {
        stateManager->setIntakePosition("open");
    } else {
        stateManager->setIntakePosition("float");
    }

    if (controls->getCodriverButton(9)) {
        stateManager->brakeClaw();
    }

    if (controls->getCodriverButton(10)) {
        stateManager->setRepack();
    }

    if (controls->getCodriverDpadUp()) {
        stateManager->incrementElevatorHeight(5);
    }

    if (controls->getCodriverDpadDown()) {
        stateManager->incrementElevatorHeight(-5);
    }
}

} /* namespace frc973 */
