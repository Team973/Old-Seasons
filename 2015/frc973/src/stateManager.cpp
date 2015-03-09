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
    lastRobotState = IDLE;
    internalState = RUNNING;

    leftIntakeSpeed = 0;
    rightIntakeSpeed = 0;

    hadTote = false;

    wantContainer = false;
    wantAutoLoad = false;
    wantHumanLoad = false;

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

void StateManager::setHigh() {
    sauropod->setPreset("loadHigh");
}

void StateManager::setScore() {
    sauropod->setPreset("hardStop");
    robotState = SCORE;
}

void StateManager::setAutoLoad(bool wantLoad) {
    if (wantLoad) {
        robotState = AUTO_LOAD;
        restingPath = "loadHigh";
    } else if (wantAutoLoad && !wantLoad) {
        internalState = END;
    }
    wantAutoLoad = wantLoad;
}

void StateManager::setContainerLoad(bool wantLoad) {
    if (wantLoad) {
        robotState = CONTAINER_LOAD;
    } else if (wantContainer && !wantLoad) {
        internalState = END;
    }
    wantContainer = wantLoad;
}

void StateManager::setHumanLoad(bool wantLoad) {
    if (wantLoad) {
        robotState = HUMAN_LOAD;
    } else if (wantHumanLoad && !wantLoad) {
        internalState = END;
    }
    wantHumanLoad = wantLoad;
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

    if (robotState != lastRobotState) {
        internalState = RUNNING;
    }

    switch (robotState) {
        case AUTO_LOAD:
            switch (internalState) {
                case RUNNING:
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
                        if (restingPath == "rest") {
                            internalState = DEAD;
                        }
                    } else if (!intake->gotTote() && !hadTote && !sauropod->inCradle()) {
                        lockTimer->Stop();
                        lockTimer->Reset();
                        drive->unlock();
                    }
                    break;
                case END:
                    restingPath = "rest";
                    internalState = RUNNING;
                    break;
                case DEAD:
                    break;
            }

            if (intake->gotTote() && leftIntakeSpeed < 0 && rightIntakeSpeed < 0) {
                intake->setIntake(0);
            }

            break;
        case CONTAINER_LOAD:
            switch (internalState) {
                case RUNNING:
                    break;
                case END:
                    break;
            }
            break;
        case HUMAN_LOAD:
            switch (internalState) {
                case RUNNING:
                    restingPath = "humanLoadHigh";
                    break;
                case END:
                    restingPath = "rest";
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

    lastRobotState = robotState;

    sauropod->update();
    intake->update();
}

}
