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

    leftArm = new Arm();
    leftArm->state = IDLE;
    leftArm->motorA = leftMotorA;
    leftArm->motorB = leftMotorB;
    leftArm->angleFault = false;
    leftArm->timer = new Timer();

    rightArm = new Arm();
    rightArm->state = IDLE;
    rightArm->motorA = rightMotorA;
    rightArm->motorB = rightMotorB;
    rightArm->angleFault = false;
    rightArm->timer = new Timer();

    grabberPID = new PID(0.001,0.0,0.0);
    grabberPID->start();
    initGrabSequence();
}

void ContainerGrabber::setControlMode(Arm* arm, std::string mode) {
    if (mode == "position") {
        arm->motorA->SetControlMode(CANSpeedController::kPosition);
        arm->motorB->SetControlMode(CANSpeedController::kPosition);
    } else if (mode == "openLoop") {
        arm->motorA->SetControlMode(CANSpeedController::kPercentVbus);
        arm->motorB->SetControlMode(CANSpeedController::kPercentVbus);
    }
}

void ContainerGrabber::setPIDslot(Arm* arm, int slot) {
    arm->motorA->SelectProfileSlot(slot);
    arm->motorB->SelectProfileSlot(slot);
}

void ContainerGrabber::setPositionTarget(Arm* arm, float target) {
    leftArm->motorA->Set(target);
    leftArm->motorB->Set(target);
}

// this should only be called after the arm gets set to openloop control
void ContainerGrabber::setMotorsOpenLoop(Arm* arm, float speed) {
    arm->motorA->Set(speed);
    arm->motorB->Set(speed);
}

void ContainerGrabber::setPIDTarget(float target) {
    grabberPID->setTarget(target);
}

void ContainerGrabber::cancelGrabSequence() {
    setControlMode(leftArm, "openLoop");
    setControlMode(rightArm, "openLoop");
    setPIDTarget(0);
    initIdleState(leftArm);
    initIdleState(rightArm);
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
    leftArm->state = DROP;
    rightArm->state = DROP;
    leftArm->timer->Start();
    leftArm->timer->Reset();
    rightArm->timer->Start();
    rightArm->timer->Reset();
}

void ContainerGrabber::initSettleState(Arm* arm) {
    arm->state = SETTLE;
    setPIDslot(arm, 1);
    setPositionTarget(arm, Constants::getConstant("kGrabberSettleTarget")->getFloat());
}

void ContainerGrabber::initPullState(Arm* arm) {
    arm->state = PULL;
}

void ContainerGrabber::initIdleState(Arm* arm) {
    arm->state = IDLE;
}

void ContainerGrabber::retract() {
    leftArm->state = RETRACT;
    rightArm->state = RETRACT;
}

void ContainerGrabber::stateHandler(Arm* arm) {
    float angleFaultCheck = Constants::getConstant("kGrabberFaultAngle")->getInt();
    float faultCheckTime = Constants::getConstant("kGrabberFaultTime")->getInt();

    switch (arm->state) {
        case DROP:
            if (arm->timer->Get() > faultCheckTime) {
                if (arm->motorA->GetEncPosition() < angleFaultCheck) {
                    arm->angleFault = true;
                }
            }

            if (arm->motorA->GetEncPosition() > Constants::getConstant("kGrabberDropTransAngle")->getFloat()) {
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
            setMotorsOpenLoop(arm, grabberPID->update(arm->motorA->GetEncPosition()));
            break;
    }
}

void ContainerGrabber::update() {
    stateHandler(leftArm);
    stateHandler(rightArm);
    SmartDashboard::PutNumber("Left Arm State: ", leftArm->state);
}

} /* namespace frc973 */
