#include "WPILib.h"
#include "intake.hpp"

namespace frc973 {

Intake::Intake(VictorSP* leftIntakeMotor_, VictorSP* rightIntakeMotor_, Solenoid* floorSolenoid_, DigitalInput *toteSensor_) {
    rightIntakeMotor = rightIntakeMotor_;
    leftIntakeMotor = leftIntakeMotor_;
    floorSolenoid =  floorSolenoid_;
    toteSensor = toteSensor_;

    intakeMotorSpeed = 0;

    isFloorSolenoidExtended = false;

    toteTimer = new Timer();

    hasTote = false;
}

void Intake::setIntake(float indicatedSpeed) {
    intakeMotorSpeed = indicatedSpeed;
}

void Intake::actuateFloorSolenoids(bool actuate) {
        isFloorSolenoidExtended = actuate;
        floorSolenoid->Set(!actuate);
}

bool Intake::gotTote() {
    return hasTote;
}

void Intake::update() {
    rightIntakeMotor->Set(intakeMotorSpeed);
    leftIntakeMotor->Set(intakeMotorSpeed);

    if (!hasTote && toteSensor->Get()) {
        toteTimer->Start();
        if (toteTimer->Get() >= 0.25) {
            hasTote = true;
        }
    } else if (hasTote && !toteSensor->Get()) {
        hasTote = false;
    } else {
        toteTimer->Stop();
        toteTimer->Reset();
    }
}

} /* namespace frc973 */
