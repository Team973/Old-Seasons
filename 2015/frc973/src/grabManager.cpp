#include "WPILib.h"
#include "grabManager.hpp"
#include "subsystems/containerGrabber.hpp"
#include "subsystems/drive.hpp"

namespace frc973 {

GrabManager::GrabManager(Drive* drive_, ContainerGrabber* grabber_) {
    drive = drive_;
    grabber = grabber_;

    waitForContact = false;
}

void GrabManager::runArmsFreeSpeed() {
    grabber->runArmsFreeSpeed();
}

void GrabManager::startSequence(float speed, bool wait) {
    waitForContact = wait;
    grabber->startGrabSequence(speed);
}

void GrabManager::cancelSequence() {
    grabber->cancelGrabSequence();
}

void GrabManager::update() {
}

}
