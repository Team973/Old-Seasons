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

void StateManager::setSauropodState(int state) {
    sauropod->clearQueue();
    sauropodState = state;
}

void StateManager::setIntakeState(int state) {
    intakeState = state;
}

void StateManager::update() {

    switch (intakeState) {
        case INTAKE:
            intake->setIntake(manualIntakeSpeed);
            break;
        case INTAKE_AUTO:
            intake->setIntake(0);
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
        case SCORE_STEP:
            break;
        case SCORE_PLATFORM:
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
