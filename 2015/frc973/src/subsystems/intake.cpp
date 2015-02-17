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
    whipPID = new PID(0.3,0,0);
    whipPID->start();
    whipPID->setBounds(-1,1);
    whipPID->setTarget(243);

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
    return (360 * whipPot->GetVoltage())/5;
}

void Intake::setWhipTarget(float target) {
    whipPID->setTarget(target);

    if (target < 260) { 
        isRetracted = true;
    }
    else if (target > 260) { 
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
    
    if (isRetracted) {

        if (isFeederSolenoidExtended) {
                setWhipTarget(243);
        }
        else { 
                setWhipTarget(245);
        }
        
    }

    whipMotor->Set(-whipPID->update(getWhipAngle()));
}

} /* namespace frc973 */
