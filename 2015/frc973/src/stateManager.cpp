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
}

void StateManager::setDriveFromControls(double throttle, double turn, bool quickTurn) {
    drive->controlInterface(deadband(throttle, 0.1), deadband(turn, 0.1), false, quickTurn);
}

void StateManager::setIntakeSpeed(float speed) {
    intakeSpeed = speed;
}

void StateManager::setIntakePosition(bool open) {
    intake->actuateFloorSolenoids(open);
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

void StateManager::update() {

    intake->setIntake(intakeSpeed);

    switch (robotState) {
        case LOAD:

            // auto stack
            if (intake->gotTote() && !sauropod->inCradle() && !hadTote && sauropod->sequenceDone()) {
                hadTote = true;
                if (sauropod->getCurrPath() != Sauropod::PICKUP) {
                    setSauropodPath(Sauropod::PICKUP);
                }
            } else if (sauropod->sequenceDone() && hadTote && sauropod->inCradle()) {
                numTotes += 1;
                sauropod->setNumTotes(numTotes);
                setSauropodPath(Sauropod::READY);
                hadTote = false;
            }

            if (intake->gotTote()) {
                intake->setIntake(0);
            }

            if (numTotes >= 6) {
                robotState = IDLE;
            }

            break;
        case SCORE:
            break;
        case IDLE:
            break;
    }

    SmartDashboard::PutNumber("Num Totes: ", numTotes);
    sauropod->setNumTotes(numTotes);

    sauropod->update();
    intake->update();
}

}
