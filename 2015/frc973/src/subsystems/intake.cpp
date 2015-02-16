#include "WPILib.h"
#include "intake.hpp"

namespace frc973 {

Intake::Intake(VictorSP* leftIntakeMotor_, VictorSP* rightIntakeMotor_, VictorSP* humanFeederIntakeMotor_, VictorSP *whipMotor_, Solenoid* floorSolenoid_, Solenoid* humanFeederSolenoid_, AnalogInput *whipPot_, DigitalInput *toteSensor_) {
    
    rightIntakeMotor = rightIntakeMotor_;
    leftIntakeMotor = leftIntakeMotor_;
    humanFeederIntakeMotor = humanFeederIntakeMotor_;
    whipMotor = whipMotor_;
    floorSolenoid =  floorSolenoid_;
    humanFeederSolenoid = humanFeederSolenoid_;
    whipPot = whipPot_;
    toteSensor = toteSensor_;

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
}

} /* namespace frc973 */
