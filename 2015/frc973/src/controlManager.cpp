#include "WPILib.h"
#include "controlManager.hpp"
#include "controlMap.hpp"
#include "stateManager.hpp"
#include "grabManager.hpp"
#include "subsystems/sauropod.hpp"

namespace frc973 {

ControlManager::ControlManager(ControlMap *controls_, StateManager *stateManager_, GrabManager *grabManager_) {
    controls = controls_;
    stateManager = stateManager_;
    grabManager = grabManager_;

    clawClosedOverriden = false;

    initialRetract = false;

    capButtonDelay = false;
}

void ControlManager::update() {
    stateManager->setDriveFromControls(controls->getThrottle(), -controls->getTurn(), controls->getDriverButton(10));
    stateManager->vTecKickedInYo(controls->getDriverButton(6));

    if (controls->getDriverButton(4)) {
        stateManager->setScore();
    }

    if (controls->getDriverButton(3)) {
        stateManager->actuateClaw(false);
        clawClosedOverriden = true;
    }

    if (controls->getCodriverButton(7) && clawClosedOverriden) {
        clawClosedOverriden = false;
    }

    if (!clawClosedOverriden) {
        stateManager->actuateClaw(!controls->getCodriverButton(7));
    }



    if (controls->getDriverButton(8)) {
        stateManager->unBrakeClaw();
    }

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
        stateManager->unBrakeClaw();
    }

    stateManager->setContainerLoad(controls->getCodriverButton(6));


    stateManager->setIntakePosition(controls->getCodriverButton(8));

    if (controls->getCodriverButton(9)) {
        stateManager->brakeClaw();
    }

    if (controls->getCodriverButton(10)) {
        stateManager->setRepack();
    }

    if (controls->getCodriverButton(11) && !capButtonDelay) {
        capButtonDelay = true;
        stateManager->setCapState();
        stateManager->incrementCapHeight();
    }
    capButtonDelay = controls->getCodriverButton(11);
}

} /* namespace frc973 */
