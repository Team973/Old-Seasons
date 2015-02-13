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

    manualIntakeSpeed = 0;
}

void StateManager::setDriveFromControls(double throttle, double turn, bool quickTurn) {
    drive->CheesyDrive(deadband(throttle, 0.1), -deadband(turn, 0.1), false, quickTurn);
}

void StateManager::setIntakeFromControls(float manual) {
    manualIntakeSpeed = manual;
}

void StateManager::setSauropodPreset(std::string preset) {
    sauropod->setPreset(preset);
}

void StateManager::update() {
    intake->setIntake(manualIntakeSpeed);

    switch (intakeState) {
        case INTAKE:
            break;
        case EXHAUST:
            break;
        case IDLE:
            break;
    }

    switch (sauropodState) {
        case LOAD:
            break;
        case SCORE_STEP:
            break;
        case SCORE_PLATFORM:
            break;
    }

    sauropod->update();
    intake->update();
}

void StateManager::setElevatorPath() {
    sauropod->createPath(IDLE);
}

}
