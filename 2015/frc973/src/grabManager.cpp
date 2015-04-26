#include "WPILib.h"
#include "grabManager.hpp"
#include "subsystems/drive.hpp"

namespace frc973 {

GrabManager::GrabManager(Drive* drive_, Talon* leftArm_, Talon* rightArm_) {
    drive = drive_;

    leftArm = leftArm_;
    rightArm = rightArm_;

    timer = new Timer();

    sequenceCanceled = false;
}

void GrabManager::cancelSequence() {
    sequenceCanceled = true;
    leftArm->Set(0.0);
    rightArm->Set(0.0);
}

void GrabManager::runArms() {
    leftArm->Set(-1.0);
    rightArm->Set(-1.0);
}

void GrabManager::init() {
    timer->Start();
}

void GrabManager::update() {

    if (!sequenceCanceled) {

        if (timer->Get() <= 0.30 && timer->Get() >= 0.1) {
            drive->arcade(-1.0, 0.0);
        } else {
            drive->arcade(0.0, 0.0);
        }

        if (timer->Get() >= 0.15) {
            leftArm->Set(-0.1);
            rightArm->Set(-0.1);
        } else {
            leftArm->Set(-1.0);
            rightArm->Set(-1.0);
        }

    }

}

}
