#include "WPILib.h"
#include "containerGrabber.hpp"
#include "../lib/pid.hpp"
#include "../constants.hpp"

namespace frc973 {

ContainerGrabber::ContainerGrabber(CANTalon* leftMotorA_, CANTalon* leftMotorB_, CANTalon* rightMotorA_, CANTalon* rightMotorB_, Encoder* leftEncoder_, Encoder* rightEncoder_) {
    leftMotorA = leftMotorA_;
    leftMotorB = leftMotorB_;
    rightMotorA = rightMotorA_;
    rightMotorB = rightMotorB_;
    leftEncoder = leftEncoder_;
    rightEncoder = rightEncoder_;

    leftArm = {
        IDLE,
        leftMotorA,
        leftMotorB,
        leftEncoder,
        false,
        new Timer(),
    };

    rightArm = {
        IDLE,
        rightMotorA,
        rightMotorB,
        rightEncoder,
        false,
        new Timer(),
    };


    grabberPID = new PID(0.0,0.0,0.0);
    grabberPID->start();
}

void ContainerGrabber::testMotor(float speed) {
    /*
    motorA->Set(speed);
    motorB->Set(speed);
    */
}

void ContainerGrabber::testSetPositionTarget(float position) {
    grabberPID->setTarget(position);
}

void ContainerGrabber::testMotorClosedLoop(float position) {
    /*
    motorA->Set(grabberPID->update(encoderA->Get()));
    */
}

void ContainerGrabber::setControlMode(std::string mode) {
    if (mode == "position") {
        /*
        motorA->SetControlMode(CANSpeedController::kPosition);
        motorB->SetControlMode(CANSpeedController::kPosition);
        */
    } else if (mode == "openLoop") {
        /*
        motorA->SetControlMode(CANSpeedController::kSpeed);
        motorB->SetControlMode(CANSpeedController::kSpeed);
        */
    }
}

void ContainerGrabber::setPIDslot(int slot) {
    /*
    motorA->SelectProfileSlot(slot);
    motorB->SelectProfileSlot(slot);
    */
}

void ContainerGrabber::setPositionTarget(float target) {
    /*
    motorA->Set(target);
    motorB->Set(target);
    */
}

void ContainerGrabber::setPIDTarget(float target) {
    grabberPID->setTarget(target);
}

void ContainerGrabber::grab() {
    setControlMode("position");
    setPIDslot(0);
    setPositionTarget(Constants::getConstant("kGrabberDropTarget")->getFloat());
    leftArm.state = DROP;
    rightArm.state = DROP;
    leftArm.timer->Start();
    leftArm.timer->Reset();
    rightArm.timer->Start();
    rightArm.timer->Reset();
}

void ContainerGrabber::retract() {
    leftArm.state = RETRACT;
    rightArm.state = RETRACT;
}

void ContainerGrabber::stateHandler(Arm arm) {
    float angleFaultCheck = Constants::getConstant("kGrabberFaultAngle")->getFloat();
    float faultCheckTime = Constants::getConstant("kGrabberFaultTime")->getFloat();
    bool stateChanged = false;

    do {
        stateChanged = false;
        switch (arm.state) {
            case DROP:
                if (arm.timer->Get() > faultCheckTime) {
                    if (arm.encoder->Get() < angleFaultCheck) {
                        arm.angleFault = true;
                    }
                }

                if (arm.encoder->Get() > Constants::getConstant("kGrabberDropTarget")->getFloat()) {
                    arm.state = SETTLE;
                    stateChanged = true;
                }
                break;
            case SETTLE:
                setPIDslot(1);
                setPositionTarget(Constants::getConstant("kGrabberSettleTarget")->getFloat());
                break;
            case PULL:
                break;
            case RETRACT:
                break;
            case IDLE:
                break;
        }
    } while (stateChanged == false);
}

void ContainerGrabber::update() {
    stateHandler(leftArm);
    stateHandler(rightArm);
}

} /* namespace frc973 */
