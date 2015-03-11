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

    lastTote = false;

    lockTimer = new Timer();

    restingPath = "loadHigh";
    pickupPath = "loadLow";

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
    if (robotState == HUMAN_LOAD) {
        speed *= 0.7;
        if (speed > 0.7) {
            speed = 0.7;
        } else if (speed < -0.7) {
            speed = 0.7;
        }
    }
    leftIntakeSpeed = speed;
    rightIntakeSpeed = speed;
}

void StateManager::setIntakeLeftRight(float left, float right) {
    leftIntakeSpeed = left;
    rightIntakeSpeed = right;
}

void StateManager::setIntakePosition(std::string position) {
    if (position == "open") {
        intake->actuateIntake(true, false);
    } else if (position == "float") {
        intake->actuateIntake(false, false);
    } else if (position == "closed") {
        intake->actuateIntake(true, true);
    }
}

bool StateManager::gotTote() {
    return intake->gotTote();
}

void StateManager::setRobotState(int state) {
    robotState = state;
}

void StateManager::setScore() {
    sauropod->setPreset("hardStop");
    robotState = SCORE;
}

void StateManager::setResting() {
    sauropod->setPreset("rest");
}

void StateManager::setAutoLoad(bool wantLoad) {
    if (wantLoad) {
        robotState = AUTO_LOAD;
    } else if (wantAutoLoad && wantLoad && !lastTote) {
        restingPath = "loadHigh";
    }
    wantAutoLoad = wantLoad;
}

void StateManager::setContainerLoad(bool wantLoad) {
    if (wantLoad) {
        robotState = CONTAINER_LOAD;
    }
    wantContainer = wantLoad;
}

void StateManager::setHumanLoad(bool wantLoad) {
    if (wantLoad) {
        robotState = HUMAN_LOAD;
        if (!lastTote) {
            restingPath = "humanLoadLow";
            pickupPath = "humanLoadHigh";
        }
    }
    wantHumanLoad = wantLoad;
}

void StateManager::setLastTote() {
    lastTote = true;
    internalState = END;
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

    if (robotState != lastRobotState) {
        internalState = RUNNING;
    }
    
    switch (robotState) {
        case AUTO_LOAD:
            switch (internalState) {
                case RUNNING:
                    if (wantAutoLoad && !sauropod->isCurrPreset("loadHigh")) {
                            sauropod->setPreset("loadHigh");
                    }

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
                        if (lastTote) {
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
                    lastTote = false;
                    break;
            }

            if (intake->gotTote() && leftIntakeSpeed < 0 && rightIntakeSpeed < 0) {
                intake->setIntake(0);
            }

            break;
        case CONTAINER_LOAD:
            switch (internalState) {
                case RUNNING:
                    sauropod->setPreset("hardStop");
                    setIntakePosition("float");
                    intake->actuateFoot(true);
                    break;
                case END:
                    setIntakePosition("closed");
                    intake->actuateFoot(false);
                    internalState = DEAD;
                    break;
                case DEAD:
                    break;
            }
            break;
        case HUMAN_LOAD:
            switch (internalState) {
                case RUNNING:
                    if (wantHumanLoad && !sauropod->isCurrPreset(pickupPath) && !sauropod->isCurrPreset(restingPath) && !lastTote) {
                        sauropod->setPreset(pickupPath);
                    }

                    if (wantHumanLoad && sauropod->motionDone() && !sauropod->isCurrPreset(restingPath)) {
                        sauropod->setPreset(restingPath);
                    } else if (sauropod->motionDone() && sauropod->isCurrPreset(restingPath)) {
                        sauropod->setPreset(pickupPath);
                        if (lastTote) {
                            internalState = DEAD;
                        }
                    }

                    break;
                case END:
                    restingPath = "loadLow";
                    pickupPath = "rest";
                    internalState = RUNNING;
                    break;
                case DEAD:
                    lastTote = false;
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

    intake->setIntakeLeftRight(leftIntakeSpeed, rightIntakeSpeed);

    SmartDashboard::PutNumber("Current Internal State: ", internalState);
    SmartDashboard::PutBoolean("Last Tote: ", lastTote);
    SmartDashboard::PutString("Resting Path: ", restingPath.c_str());
    SmartDashboard::PutString("Pickup Path: ", pickupPath.c_str());

    if (lockTimer->Get() > 5) {
        drive->unlock();
    }

    lastRobotState = robotState;

    sauropod->update();
    intake->update();
}

}
