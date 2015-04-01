#include "WPILib.h"
#include "containerGrabber.hpp"
#include "../lib/pid.hpp"
#include "../constants.hpp"

namespace frc973 {

ContainerGrabber::ContainerGrabber(Solenoid* solenoid_, CANTalon* motorA_, CANTalon* motorB_, Encoder* encoderA_, Encoder* encoderB_) {
    solenoid = solenoid_;
    motorA = motorA_;
    motorB = motorB_;
    encoderA = encoderA_;
    encoderB = encoderB_;

    grabberState = IDLE;

    grabberPID = new PID(0.0,0.0,0.0);
    grabberPID->start();
}

void ContainerGrabber::testMotor(float speed) {
    motorA->Set(speed);
    //motorB->Set(speed);
}

void ContainerGrabber::testSetPositionTarget(float position) {
    grabberPID->setTarget(position);
}

void ContainerGrabber::testMotorClosedLoop(float position) {
    motorA->Set(grabberPID->update(encoderA->Get()));
}

void ContainerGrabber::setControlMode(std::string mode) {
    if (mode == "position") {
        motorA->SetControlMode(CANSpeedController::kPosition);
        motorB->SetControlMode(CANSpeedController::kPosition);
    } else if (mode == "openLoop") {
        motorA->SetControlMode(CANSpeedController::kSpeed);
        motorB->SetControlMode(CANSpeedController::kSpeed);
    }
}

void ContainerGrabber::setPIDSlot(int slot) {
    motorA->SelectProfileSlot(slot);
    motorB->SelectProfileSlot(slot);
}

void ContainerGrabber::setPIDTarget(float target) {
    motorA->Set(target);
    motorB->Set(target);
}

void ContainerGrabber::grab() {
    setControlMode("position");
    setPIDSlot(0);
    setPIDTarget(Constants::getConstant("kGrabberDropTarget")->getFloat());
    grabberState = DROP;
}

void ContainerGrabber::retract() {
    grabberState = RETRACT;
}

void ContainerGrabber::update() {
    switch (grabberState) {
        case DROP:
            break;
        case SETTLE:
            break;
        case PULL:
            break;
        case RETRACT:
            break;
        case IDLE:
            break;
    }
}

} /* namespace frc973 */
