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
    intakeState = INTAKE_IDLE;
    sauropodState = SAUROPOD_IDLE;

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
}

void StateManager::update() {
    intake->setIntake(manualIntakeSpeed);

    switch (robotState) {
        case SCORE_STEP:
            break;
        case SCORE_PLATFORM:
            break;
        case IDLE: // this state will transition to any other state depending on the situation
            break;
    }

    switch (intakeState) {
        case INTAKE:
            break;
        case EXHAUST:
            break;
        case HUMAN_LOAD:
            break;
        case INTAKE_IDLE:
            break;
    }

    switch (sauropodState) {
        case LOAD:
            sauropod->createPath(Sauropod::PICKUP);
            break;
        case STEP:
            break;
        case PLATFORM:
            sauropod->createPath(Sauropod::PLATFORM);
            break;
        case SAUROPOD_IDLE:
            sauropod->createPath(Sauropod::IDLE);
            break;
    }

    sauropod->update();
    intake->update();
}

}
