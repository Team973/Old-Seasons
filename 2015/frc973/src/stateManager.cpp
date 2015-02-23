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
    numTotes = 0;

    lockTimer = new Timer();

    isAutoStack = true;

    vTec_yo = false;
}

void StateManager::vTecKickedInYo(bool kickedInYo) {
    vTec_yo = kickedInYo;
}

void StateManager::setDriveFromControls(double throttle, double turn, bool quickTurn) {
    if (vTec_yo) {
        drive->CheesyDrive(deadband(throttle, 0.1), deadband(turn, 0.1), false, quickTurn);
    } else {
        drive->controlInterface(deadband(throttle, 0.1), deadband(turn, 0.1), false, quickTurn);
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
    switch (path) {
        case Sauropod::READY:
            robotState = LOAD;
        break;
        case Sauropod::RESTING:
            robotState = IDLE;
        break;
    }
}

void StateManager::setWhipPosition(std::string position) {
    if (position == "extend") {
        intake->extendWhip();
    } else {
        intake->retractWhip();
    }
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

void StateManager::update() {

    intake->setIntake(intakeSpeed);

    switch (robotState) {
        case LOAD:

            // auto stack
            //if (isAutoStack) {
                if (intake->gotTote() && !sauropod->inCradle() && !hadTote && sauropod->sequenceDone()) {
                    hadTote = true;
                    drive->lock();
                    lockTimer->Start();
                    if (sauropod->getCurrPath() != Sauropod::PICKUP) {
                        setSauropodPath(Sauropod::PICKUP);
                    }
                } else if (sauropod->sequenceDone() && hadTote && sauropod->inCradle()) {
                    numTotes += 1;
                    sauropod->setNumTotes(numTotes);
                    setSauropodPath(Sauropod::READY);
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

                if (numTotes >= 6) {
                    robotState = IDLE;
                }
            //}

            break;
        case SCORE:
            break;
        case IDLE:
            numTotes = 0;
            break;
    }

    SmartDashboard::PutNumber("Num Totes: ", numTotes);
    sauropod->setNumTotes(numTotes);

    sauropod->update();
    intake->update();
}

}
