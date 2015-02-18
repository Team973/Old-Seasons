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

    fromControls = false;

    robotState = IDLE;

    manualIntakeSpeed = 0;

    numTotes = 0;
}

void StateManager::setDriveFromControls(double throttle, double turn, bool quickTurn) {
    drive->CheesyDrive(deadband(throttle, 0.1), -deadband(turn, 0.1), false, quickTurn);
}

void StateManager::setIntakeFromControls(float manual) {
    manualIntakeSpeed = manual;
}

void StateManager::setRobotState(int state) {
    robotState = state;
    fromControls = true;
}

void StateManager::setSauropodPath(int path) {
    if (fromControls) {
        sauropod->createPath(path);
        fromControls = false;
    } else {
        fromControls = true;
    }
}

void StateManager::setWhipPosition(float position) {
    intake->setWhipTarget(position);
}

bool StateManager::isSauropodDone() {
    return sauropod->sequenceDone();
}

void StateManager::update() {

    switch (robotState) {
        case LOAD:
            /*
            intake->setIntake(manualIntakeSpeed);

            if (intake->gotTote() && !sauropod->inCradle()) {
                intake->setIntake(0);
                if (sauropod->getCurrPath() != Sauropod::PICKUP) {
                    setSauropodPath(Sauropod::PICKUP);
                }
            } else if (sauropod->sequenceDone() && sauropod->getCurrPath() == Sauropod::PICKUP) {
                numTotes += 1;
                setSauropodPath(Sauropod::READY);
            }
            */

            break;
        case SCORE:
            break;
        case IDLE:
            intake->setIntake(0);
            break;
    }
    sauropod->update();
    intake->update();
}

}
