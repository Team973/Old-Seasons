#include "WPILib.h"
#include "containerGrabber.hpp"
#include "../lib/pid.hpp"
#include "../lib/utility.hpp"
#include "../lib/logger.hpp"
#include "../constants.hpp"

namespace frc973 {

ContainerGrabber::ContainerGrabber(CANTalon* leftMotorA_, CANTalon* leftMotorB_, CANTalon* rightMotorA_, CANTalon* rightMotorB_) {
    leftMotorA = leftMotorA_;
    leftMotorB = leftMotorB_;
    rightMotorA = rightMotorA_;
    rightMotorB = rightMotorB_;

    leftArm = new Arm();
    leftArm->state = HOME;
    leftArm->motorA = leftMotorA;
    leftArm->motorB = leftMotorB;
    leftArm->angleFault = false;
    leftArm->contact = false;
    leftArm->atDriveAngle = false;

    rightArm = new Arm();
    rightArm->state = HOME;
    rightArm->motorA = rightMotorA;
    rightArm->motorB = rightMotorB;
    rightArm->angleFault = false;
    rightArm->contact = false;
    rightArm->atDriveAngle = false;

    goinSlow = false;
    limitSpeed = 1.0;

    angleFaultCheck = Constants::getConstant("kGrabberFaultAngle")->getInt();

    driveAngle = 0;
    slowDriveAngle = 100;
    fastDriveAngle = 150;

    dropTransitionAngle = Constants::getConstant("kGrabberDropTransAngle")->getFloat();
    dropTargetAngle = Constants::getConstant("kGrabberDropTarget")->getFloat();

    settleTargetAngle = Constants::getConstant("kGrabberSettleTarget")->getFloat();

    settleSpeed = 0.8;

    grabberPID = new PID(0.001,0.0,0.0);
    grabberPID->start();

    homePID = new PID(0.001,0.0,0.0);
    homePID->start();
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
    arm->motorA->Set(target);
    arm->motorB->Set(target);
}

// this should only be called after the arm gets set to openloop control
void ContainerGrabber::setMotorsOpenLoop(Arm* arm, float speed) {
    if (speed > limitSpeed) {
        speed = limitSpeed;
    }
    arm->motorA->Set(speed);
    arm->motorB->Set(speed);
}

void ContainerGrabber::setPIDTarget(float target) {
    grabberPID->setTarget(target);
}

void ContainerGrabber::runArmsFreeSpeed() {
    setControlMode(leftArm, "openLoop");
    setControlMode(rightArm, "openLoop");
    setMotorsOpenLoop(leftArm, 1.0);
    setMotorsOpenLoop(rightArm, 1.0);
}

void ContainerGrabber::cancelGrabSequence() {
    setControlMode(leftArm, "openLoop");
    setControlMode(rightArm, "openLoop");
    setPIDTarget(0);
    initHomeState(leftArm);
    initHomeState(rightArm);
}

void ContainerGrabber::initGrabSequence() {
    setControlMode(leftArm, "position");
    setControlMode(rightArm, "position");
    setPIDslot(leftArm, 0);
    setPIDslot(rightArm, 0);
    setPositionTarget(leftArm, dropTargetAngle);
    setPositionTarget(rightArm, dropTargetAngle);
}

void ContainerGrabber::startGrabSequence(float speed) {
    if (speed < 1.0) {
        limitSpeed = speed;
        goinSlow = true;
        setControlMode(leftArm, "openLoop");
        setControlMode(rightArm, "openLoop");
        driveAngle = slowDriveAngle;
        initSlowState(leftArm);
        initSlowState(rightArm);
    } else {
        leftArm->state = DROP;
        rightArm->state = DROP;
        driveAngle = fastDriveAngle;
    }
}

void ContainerGrabber::initSettleState(Arm* arm) {
    arm->state = SETTLE;
    arm->contact = true;
    setControlMode(arm, "openLoop");
    setMotorsOpenLoop(arm, settleSpeed);
}

void ContainerGrabber::initSlowState(Arm* arm) {
    arm->state = SLOW;
    setControlMode(arm, "openLoop");
    setMotorsOpenLoop(arm, 1.0);
}

void ContainerGrabber::initHomeState(Arm* arm) {
    arm->state = HOME;
    homePID->setTarget(0);
}

void ContainerGrabber::retract() {
    setControlMode(leftArm, "openLoop");
    setControlMode(rightArm, "openLoop");
    setPIDTarget(0);
    leftArm->state = RETRACT;
    rightArm->state = RETRACT;
}

bool ContainerGrabber::isSettled() {
    return leftArm->state == SETTLE && rightArm->state == SETTLE;
}

bool ContainerGrabber::haveBothContact() {
    return leftArm->contact && rightArm->contact;
}

bool ContainerGrabber::gotFault() {
    return leftArm->angleFault || rightArm->angleFault;
}

bool ContainerGrabber::bothAtDriveAngle() {
    //return leftArm->atDriveAngle && rightArm->atDriveAngle;
    return rightArm->atDriveAngle;
}

void ContainerGrabber::stateHandler(Arm* arm) {

    printf("%d\n", arm->motorA->GetEncPosition());
    switch (arm->state) {
        case DROP:
            if (arm->motorA->GetEncPosition() < angleFaultCheck) {
                arm->angleFault = true;
            }

            if (arm->motorA->GetEncPosition() > dropTransitionAngle) {
                initSettleState(arm);
            }
            break;
        case SETTLE:
            setMotorsOpenLoop(arm, settleSpeed);
            break;
        case RETRACT:
            if (arm->motorA->GetEncPosition() <= 10) {
                initHomeState(arm);
            }

            if (arm->motorA->GetEncPosition() > 75) {
                setMotorsOpenLoop(arm, -0.2);
            } else {
                setMotorsOpenLoop(arm, -0.07);
            }
            break;
        case SLOW:
            if (arm->motorA->GetEncPosition() >= dropTargetAngle) {
                setMotorsOpenLoop(arm, 0.5);
            } else {
                setMotorsOpenLoop(arm, 1.0);
            }
            break;
        case HOME:
            setMotorsOpenLoop(arm, homePID->update(arm->motorA->GetEncPosition()) - 0.05);
            break;
    }

    if (arm->motorA->GetEncPosition() >= driveAngle) {
        arm->atDriveAngle = true;
    }

    arm->contact = true;

    /*
    if (arm->motorA->GetEncPosition() >= 320) {
        setMotorsOpenLoop(arm, 0.0);
    }
    */
}

void ContainerGrabber::update() {
    stateHandler(leftArm);
    stateHandler(rightArm);
    SmartDashboard::PutNumber("Left Arm State: ", leftArm->state);
}

} /* namespace frc973 */
