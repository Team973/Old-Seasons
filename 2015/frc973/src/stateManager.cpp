#include "WPILib.h"
#include "stateManager.hpp"
#include "lib/utility.hpp"
#include "subsystems/drive.hpp"
#include "subsystems/sauropod.hpp"
#include "subsystems/intake.hpp"

namespace frc973 {

StateManager::StateManager(Drive *drive_, Sauropod *sauropod_, Intake *intake_) {
    drive = drive_;
    sauropod = sauropod_;
    intake = intake_;

    robotState = IDLE;

    intakeSpeed = 0;

    hadTote = false;

    lockTimer = new Timer();

    isAutoStack = true;

    wantCap = false;

    restingPath = Sauropod::READY;
    pickupPath = Sauropod::CONTAINER;

    vTec_yo = false;
}

void StateManager::vTecKickedInYo(bool kickedInYo) {
    vTec_yo = kickedInYo;
}

void StateManager::setDriveFromControls(double throttle, double turn, bool lowGear) {
    if (vTec_yo) {
        drive->arcade(deadband(throttle, 0.1), deadband(turn, 0.1));
    } else {
        drive->controlInterface(deadband(throttle, 0.1), deadband(turn, 0.1), lowGear);
    }
}

void StateManager::disableAutoStacking() {
    isAutoStack = false;
}

void StateManager::enableAutoStacking() {
    isAutoStack = true;
}

void StateManager::setIntakeSpeed(float speed) {
    intakeSpeed = speed;
}

void StateManager::setIntakePosition(bool open) {
    intake->actuateFloorSolenoids(open);
}

void StateManager::dropHumanIntake(bool dropped) {
    intake->actuateHumanFeederSolenoids(dropped);
}

bool StateManager::gotTote() {
    return intake->gotTote();
}

void StateManager::setRobotState(int state) {
    robotState = state;
}

void StateManager::setSauropodPath(int path) {
    sauropod->createPath(path);
}

void StateManager::setLoadReady() {
    setSauropodPath(Sauropod::READY);
    robotState = IDLE;
}

void StateManager::setAutoLoadReady() {
    setSauropodPath(Sauropod::READY);
    robotState = LOAD;
}

void StateManager::setLastTote(bool lastTote) {
    if (lastTote) {
        restingPath = Sauropod::RESTING;
    } else {
        restingPath = Sauropod::READY;
    }
}

void StateManager::setContainerPickup() {
    setSauropodPath(Sauropod::CONTAINER);
    robotState = IDLE;
}

void StateManager::setWhipPosition(std::string position) {
    if (position == "extend") {
        intake->extendWhip();
    } else if (position == "retract") {
        intake->retractWhip();
    } else {
        intake->stowWhip();
    }
}

void StateManager::setCap(bool wantCap_) {
    if (wantCap_) {
        setSauropodPath(Sauropod::CAP);
        robotState = CAPPING;
    }
    wantCap = wantCap_;
}

bool StateManager::isSauropodDone() {
    return sauropod->sequenceDone();
}

bool StateManager::isWhipDone() {
    return intake->whipDone();
}

bool StateManager::isDriveLocked() {
    return drive->isLocked();
}

void StateManager::lockDrive() {
    drive->lock();
}

void StateManager::unlockDrive() {
    drive->unlock();
}

void StateManager::update() {

    intake->setIntake(intakeSpeed);

    switch (robotState) {
        case LOAD:
            //
            // auto stack
            if (isAutoStack) {
                if (intake->gotTote() && !sauropod->inCradle() && !hadTote && sauropod->sequenceDone()) {
                    hadTote = true;
                    drive->lock();
                    lockTimer->Start();
                    if (sauropod->getCurrPath() != Sauropod::PICKUP) {
                        setSauropodPath(Sauropod::PICKUP);
                    }
                } else if (sauropod->sequenceDone() && hadTote && sauropod->inCradle()) {
                    setSauropodPath(restingPath);
                    hadTote = false;
                } else if (!intake->gotTote() && !hadTote && !sauropod->inCradle()) {
                    lockTimer->Stop();
                    lockTimer->Reset();
                    drive->unlock();
                }

                if (lockTimer->Get() > 5) {
                    drive->unlock();
                }

                if (intake->gotTote() && intakeSpeed < 0) {
                    intake->setIntake(0);
                }
            }

            break;
        case CAPPING:
            if (!wantCap) {
                setSauropodPath(Sauropod::DROP_CAP);
                robotState = IDLE;
            }
            break;
        case SCORE:
            drive->unlock();
            break;
        case IDLE:
            drive->unlock();
            break;
    }

    if (lockTimer->Get() > 5) {
        drive->unlock();
    }

    sauropod->update();
    intake->update();
}

}
