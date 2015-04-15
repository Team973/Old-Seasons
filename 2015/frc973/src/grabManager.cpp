#include "WPILib.h"
#include "grabManager.hpp"
#include "subsystems/containerGrabber.hpp"
#include "subsystems/drive.hpp"
#include "subsystems/xyManager.hpp"
#include "subsystems/locator.hpp"

namespace frc973 {

GrabManager::GrabManager(Drive* drive_, ContainerGrabber* grabber_) {
    drive = drive_;
    grabber = grabber_;
    xyManager = XYManager::getInstance();

    timer = new Timer();

    waitForContact = false;
    goinSlow = false;
    driving = false;
}

void GrabManager::runArmsFreeSpeed() {
    grabber->runArmsFreeSpeed();
}

void GrabManager::startSequence(float speed, bool wait) {
    waitForContact = wait;
    goinSlow = speed < 1.0;
    grabber->startGrabSequence(speed);
}

void GrabManager::cancelSequence() {
    grabber->cancelGrabSequence();
}

bool GrabManager::isDriving() {
    return driving;
}

void GrabManager::update() {
    grabber->update();

    if (grabber->gotFault()) {
        waitForContact = true;
    }


    if (waitForContact) {
        if (grabber->haveBothContact()) {
            //drive->arcade(1.0, 0.0);
            driving = true;
            timer->Start();
        }
    } else if (grabber->bothAtDriveAngle() && !driving) {
        //drive->arcade(1.0, 0.0);
        driving = true;
        timer->Start();
    }

    if (timer->Get() >= 3.0 && !driving) {
        driving = true;
        //xyManager->setTargetDistance(8.0);
    }

    if (driving) {
        //drive->update();
    }

}

}
