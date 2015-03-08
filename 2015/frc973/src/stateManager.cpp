#include "WPILib.h"
#include "stateManager.hpp"
#include "lib/utility.hpp"
#include "subsystems/drive.hpp"
#include "subsystems/sauropod.hpp"
#include "subsystems/intake.hpp"
#include "subsystems/containerGrabber.hpp"

namespace frc973 {

StateManager::StateManager(Drive *drive_, Sauropod *sauropod_, Intake *intake_, ContainerGrabber *grabber_) {
    drive = drive_;
    sauropod = sauropod_;
    intake = intake_;
    grabber = grabber_;

    robotState = IDLE;
    containerState = CONTAINER_READY;

    leftIntakeSpeed = 0;
    rightIntakeSpeed = 0;

    hadTote = false;

    wantContainer = false;

    lockTimer = new Timer();

    restingPath = "loadHigh";
    pickupPath = "containerLoad";

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

void StateManager::setElevatorFromControls(float speed) {
    sauropod->setElevatorManual(deadband(speed, 0.2));
}

void StateManager::incrementElevatorHeight(float increment) {
    sauropod->incrementElevator(increment);
}

void StateManager::dropGrabber() {
    grabber->grab();
}

void StateManager::raiseGrabber() {
    grabber->retract();
}

void StateManager::setIntakeSpeed(float speed) {
    leftIntakeSpeed = speed;
    rightIntakeSpeed = speed;
}

void StateManager::setIntakeLeftRight(float left, float right) {
    leftIntakeSpeed = left;
    rightIntakeSpeed = right;
}

void StateManager::setIntakePosition(bool open) {
    intake->actuateFloorSolenoids(open);
}

bool StateManager::gotTote() {
    return intake->gotTote();
}

void StateManager::setRobotState(int state) {
    robotState = state;
}

void StateManager::setLoadReady() {
    sauropod->setPreset("loadHigh");
    restingPath = "loadHigh";
    robotState = IDLE;
}

void StateManager::setAutoLoadReady() {
    sauropod->setPreset("loadHigh");
    restingPath = "loadHigh";
    robotState = AUTO_LOAD;
}

void StateManager::setLastTote(bool lastTote) {
    if (lastTote) {
        restingPath = "rest";
    } else {
        restingPath = "loadHigh";
    }
}

void StateManager::setContainerPickup(bool wantLoad) {
    if (wantLoad) {
        robotState = CONTAINER_LOAD;
        containerState = CONTAINER_READY;
    } else if (wantContainer && !wantLoad) {
        containerState = CONTAINER_RAISE;
    }
    wantContainer = wantLoad;
}

void StateManager::setRestingLoad() {
    sauropod->setPreset("rest");
    robotState = IDLE;
}

bool StateManager::isSauropodDone() {
    return sauropod->motionDone();
}

void StateManager::setSauropodPreset(std::string name) {
    sauropod->setPreset(name);
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

    intake->setIntakeLeftRight(leftIntakeSpeed, rightIntakeSpeed);

    switch (robotState) {
        case AUTO_LOAD:
            // auto stack
            if (intake->gotTote() && !sauropod->inCradle() && !hadTote && sauropod->motionDone()) {
                hadTote = true;
                drive->lock();
                lockTimer->Start();
                if (sauropod->getCurrPreset() != "loadLow") {
                    sauropod->setPreset("loadLow");
                }
            } else if (sauropod->motionDone() && hadTote && sauropod->inCradle()) {
                sauropod->setPreset(restingPath);
                hadTote = false;
            } else if (!intake->gotTote() && !hadTote && !sauropod->inCradle()) {
                lockTimer->Stop();
                lockTimer->Reset();
                drive->unlock();
            }

            if (lockTimer->Get() > 5) {
                drive->unlock();
            }

            if (intake->gotTote() && leftIntakeSpeed < 0 && rightIntakeSpeed < 0) {
                intake->setIntake(0);
            }

            break;
        case CONTAINER_LOAD:
            switch (containerState) {
                case CONTAINER_READY:
                    break;
                case CONTAINER_RAISE:
                    break;
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
