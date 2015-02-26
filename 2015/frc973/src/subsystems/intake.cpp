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
    whipPID = new PID(0.06,0,0);
    whipPID->start();
    whipPID->setBounds(-1,1);
    whipPID->setTarget(107);

    isFeederSolenoidExtended = false;
    isFloorSolenoidExtended = false;
    isRetracted = false;

    toteTimer = new Timer();

    hasTote = false;
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
    float offset = 872;
    return (5 * ((360 * whipPot->GetVoltage())/5)) - offset;
}

void Intake::retractWhip() {
    whipPID->setTarget(107);
    isRetracted = false;
}

void Intake::extendWhip() {
    whipPID->setTarget(211);
    isRetracted = false;
}

void Intake::stowWhip() {
    whipPID->setTarget(0);
    isRetracted = true;
}

bool Intake::gotTote() {
    return hasTote;
}

bool Intake::whipDone() {
    return fabs(whipPID->getTarget() - getWhipAngle()) < 3;
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
        }
        else { 
        }
        
    }

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

    whipMotor->Set(-whipPID->update(getWhipAngle()));
}

} /* namespace frc973 */
