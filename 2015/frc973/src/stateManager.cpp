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
    drive->CheesyDrive(deadband(throttle, 0.1), -deadband(turn, 0.1), false, quickTurn);
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

void StateManager::update() {

    switch (robotState) {
        case LOAD:

            intake->setIntake(intakeSpeed);

            // auto stack
            if (intake->gotTote() && !sauropod->inCradle()) {
                hadTote = true;
                intake->setIntake(0);
                if (sauropod->getCurrPath() != Sauropod::PICKUP) {
                    setSauropodPath(Sauropod::PICKUP);
                }
            } else if (sauropod->sequenceDone() && hadTote && sauropod->inCradle()) {
                numTotes += 1;
                setSauropodPath(Sauropod::READY);
                hadTote = false;
            }

            break;
        case SCORE:
            break;
        case IDLE:
            intake->setIntake(0);
            break;
    }

    // num totes logic check
    if (!sauropod->lotsoTotes() && numTotes > 3) {
        numTotes = 0;
    } else if (sauropod->lotsoTotes() && numTotes < 3) {
        numTotes = 3;
    }
    sauropod->setNumTotes(numTotes);

    sauropod->update();
    intake->update();
}

}
