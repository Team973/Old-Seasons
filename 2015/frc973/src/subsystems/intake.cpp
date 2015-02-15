#include "WPILib.h"
#include "intake.hpp"

namespace frc973 {

Intake::Intake(VictorSP* leftIntakeMotor_, VictorSP* rightIntakeMotor_, VictorSP* humanFeederIntakeMotor_, Solenoid* floorSolenoid_, Solenoid* humanFeederSolenoid_) {
    rightIntakeMotor = rightIntakeMotor_;
    leftIntakeMotor = leftIntakeMotor_;
    humanFeederIntakeMotor = humanFeederIntakeMotor_;
    floorSolenoid =  floorSolenoid_;
    humanFeederSolenoid = humanFeederSolenoid_;
    motorSpeed = 0;
    feederSolenoidCondtion = false;
    floorSolenoidCondtion = false;
}

void Intake::setIntake(float indicatedSpeed) {
    motorSpeed = indicatedSpeed;
}

void Intake::actuateFloorSolenoids(bool actuate) {
        floorSolenoidCondtion = actuate;
        floorSolenoid->Set(actuate);
}
void Intake::actuateHumanFeederSolenoids(bool actuate) {
        feederSolenoidCondtion = actuate;
        humanFeederSolenoid->Set(actuate);
}

void Intake::update() {
    rightIntakeMotor->Set(motorSpeed);
    leftIntakeMotor->Set(motorSpeed);
    humanFeederIntakeMotor->Set(motorSpeed);
    floorSolenoid->Set(floorSolenoidCondtion);
    humanFeederSolenoid->Set(feederSolenoidCondtion);
}

} /* namespace frc973 */
