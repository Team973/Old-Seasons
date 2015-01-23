#include "WPILib.h"
#include "intake.hpp"

namespace frc973 {

Intake::Intake(VictorSP *rightMotor_, VictorSP *leftMotor_) {
    rightMotor = rightMotor_;
    leftMotor = leftMotor_;
    motorSpeed = 0;
}

void Intake::setIntake(float indicatedSpeed) {
    motorSpeed = indicatedSpeed;
}

void Intake::update() {
    rightMotor->Set(motorSpeed);
    leftMotor->Set(motorSpeed);
}

} /* namespace frc973 */
