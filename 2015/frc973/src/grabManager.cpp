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

    sequenceCanceled = false;

    waitForContact = false;
    goinSlow = false;
    driving = false;
}

void GrabManager::runArmsFreeSpeed() {
    grabber->runArmsFreeSpeed();
}

void GrabManager::initSequence() {
    grabber->initGrabSequence();
}

void GrabManager::injectArmType(int type) {
    grabber->injectArmType(type);
}

void GrabManager::startSequence(float speed, bool wait) {
    waitForContact = wait;
    goinSlow = speed < 1.0;
    grabber->startGrabSequence(speed);
}

void GrabManager::cancelSequence() {
    grabber->cancelGrabSequence();
    sequenceCanceled = true;
}

bool GrabManager::isDriving() {
    return driving;
}

void GrabManager::update() {
    grabber->update();
    if (!sequenceCanceled) {

        /*
           if (grabber->gotFault()) {
           waitForContact = true;
           }
           */

        SmartDashboard::PutString("HIT", "1");

        SmartDashboard::PutBoolean("Both at drive angle: ", grabber->bothAtDriveAngle());
        SmartDashboard::PutBoolean("Both at contact: ", grabber->haveBothContact());
        waitForContact = false;
        if (waitForContact) {
            if (grabber->haveBothContact()) {
                SmartDashboard::PutString("HIT", "2");
                drive->arcade(-1.0, 0.0);
                timer->Start();
            }
        } else if (grabber->bothAtDriveAngle()) {
            SmartDashboard::PutString("HIT", "3");
            drive->arcade(-1.0, 0.0);
            timer->Start();
        }

        if (timer->Get() >= 1.0 && !driving) {
            //driving = true;
            SmartDashboard::PutString("HIT", "4");
            drive->arcade(0.0, 0.0);
            //xyManager->setTargetDistance(8.0);
        }

        if (driving) {
            //drive->update();
        }
    }

}

}
