#include "WPILib.h"
#include "intake.hpp"
#include "../lib/pid.hpp"

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

    intakeMotorSpeed = 0;
    whipMotorSpeed = 0;
    whipPID = new PID(0,0,0);

    isFeederSolenoidExtended = false;
    isFloorSolenoidExtended = false;
    isRetracted = false;
}

void Intake::setIntake(float indicatedSpeed) {
    intakeMotorSpeed = indicatedSpeed;
}

void Intake::actuateFloorSolenoids(bool actuate) {
        isFloorSolenoidExtended = actuate;
        floorSolenoid->Set(actuate);
}
void Intake::actuateHumanFeederSolenoids(bool actuate) {
        isFeederSolenoidExtended = actuate;
        humanFeederSolenoid->Set(actuate);
}

float Intake::getWhipAngle() {
    int numTurns = 5;
    return whipPot->GetVoltage() * 360 * (numTurns/5);
}

void Intake::setWhipTarget(float target) {
    whipPID->setTarget(target);

    if (target < 5) { 
        isRetracted = true;
    }
    else if (target > 5) { 
        isRetracted = false;
    }
}

void Intake::update() {
    rightIntakeMotor->Set(intakeMotorSpeed);
    leftIntakeMotor->Set(intakeMotorSpeed);

    if (isFeederSolenoidExtended) {
        humanFeederIntakeMotor->Set(intakeMotorSpeed);
    }
    else {
        humanFeederIntakeMotor->Set(0);
    }
    
    if (getWhipAngle() > 3 && isRetracted) {

        if (isFeederSolenoidExtended) {
                setWhipTarget(3);
        }
        else { 
                setWhipTarget(2);
        }
        
    }

    whipMotor->Set(whipPID->update(whipPot->GetVoltage()));
}

} /* namespace frc973 */
