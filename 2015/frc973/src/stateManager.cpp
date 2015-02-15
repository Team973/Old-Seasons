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

bool StateManager::isSauropodDone() {
    return sauropod->sequenceDone();
}

void StateManager::update() {

    switch (robotState) {
        case LOAD:
            if (fromControls) {
                sauropod->createPath(Sauropod::PICKUP);
                fromControls = false;
            }
            intake->setIntake(manualIntakeSpeed);
            break;
        case SCORE:
            sauropod->createPath(Sauropod::PLATFORM);
            break;
        case IDLE:
            sauropod->createPath(Sauropod::IDLE);
            intake->setIntake(0);
            break;
    }
    sauropod->update();
    intake->update();
}

}
