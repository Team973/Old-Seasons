#include "WPILib.h"
#include "containerGrabber.hpp"
#include "../lib/pid.hpp"
#include "../constants.hpp"

namespace frc973 {

ContainerGrabber::ContainerGrabber(CANTalon* leftMotorA_, CANTalon* leftMotorB_, CANTalon* rightMotorA_, CANTalon* rightMotorB_) {
    leftMotorA = leftMotorA_;
    leftMotorB = leftMotorB_;
    rightMotorA = rightMotorA_;
    rightMotorB = rightMotorB_;

    leftArm = {
        IDLE,
        leftMotorA,
        leftMotorB,
        false,
        new Timer(),
    };

    rightArm = {
        IDLE,
        rightMotorA,
        rightMotorB,
        false,
        new Timer(),
    };

    grabberPID = new PID(0.001,0.0,0.0);
    grabberPID->start();
    initGrabSequence();
}

void ContainerGrabber::testMotor(float speed) {
    leftArm.motorA->Set(speed);
}

void ContainerGrabber::testSetPositionTarget(float position) {
    grabberPID->setTarget(position);
}

void ContainerGrabber::testMotorClosedLoop() {
    leftArm.motorA->Set(-grabberPID->update(-leftArm.motorA->GetEncPosition()));
}

ContainerGrabber::Arm ContainerGrabber::testGetArm(int arm) {
    if (arm == 1) {
        return leftArm;
    } else if (arm == 2) {
        return rightArm;
    }
    return leftArm;
}

void ContainerGrabber::setControlMode(Arm arm, std::string mode) {
    if (mode == "position") {
        arm.motorA->SetControlMode(CANSpeedController::kPosition);
        arm.motorB->SetControlMode(CANSpeedController::kPosition);
    } else if (mode == "openLoop") {
        arm.motorA->SetControlMode(CANSpeedController::kPercentVbus);
        arm.motorB->SetControlMode(CANSpeedController::kPercentVbus);
    }
}

void ContainerGrabber::setPIDslot(Arm arm, int slot) {
    arm.motorA->SelectProfileSlot(slot);
    arm.motorB->SelectProfileSlot(slot);
}

void ContainerGrabber::setPositionTarget(Arm arm, float target) {
    leftArm.motorA->Set(target);
    leftArm.motorB->Set(target);
}

void ContainerGrabber::setPIDTarget(float target) {
    grabberPID->setTarget(target);
}

void ContainerGrabber::initGrabSequence() {
    setControlMode(leftArm, "position");
    setControlMode(rightArm, "position");
    setPIDslot(leftArm, 0);
    setPIDslot(rightArm, 0);
    setPositionTarget(leftArm, Constants::getConstant("kGrabberDropTarget")->getFloat());
    setPositionTarget(rightArm, Constants::getConstant("kGrabberDropTarget")->getFloat());
}

void ContainerGrabber::startGrabSequence() {
    leftArm.state = DROP;
    rightArm.state = DROP;
    leftArm.timer->Start();
    leftArm.timer->Reset();
    rightArm.timer->Start();
    rightArm.timer->Reset();
}

void ContainerGrabber::initSettleState(Arm arm) {
    arm.state = SETTLE;
    setPIDslot(arm, 1);
    setPositionTarget(arm, Constants::getConstant("kGrabberSettleTarget")->getFloat());
}

void ContainerGrabber::initPullState(Arm arm) {
    arm.state = PULL;
}

void ContainerGrabber::initIdleState(Arm arm) {
    arm.state = IDLE;
}

void ContainerGrabber::retract() {
    leftArm.state = RETRACT;
    rightArm.state = RETRACT;
}

void ContainerGrabber::stateHandler(Arm arm) {
    float angleFaultCheck = Constants::getConstant("kGrabberFaultAngle")->getInt();
    float faultCheckTime = Constants::getConstant("kGrabberFaultTime")->getInt();

    switch (arm.state) {
        case DROP:
            if (arm.timer->Get() > faultCheckTime) {
                if (arm.motorA->GetEncPosition() < angleFaultCheck) {
                    arm.angleFault = true;
                }
            }

            if (arm.motorA->GetEncPosition() > 200) {
                initSettleState(arm);
            }
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

void ContainerGrabber::update() {
    stateHandler(leftArm);
    //stateHandler(rightArm);
    SmartDashboard::PutNumber("Left Arm State: ", leftArm.state);
}

} /* namespace frc973 */
