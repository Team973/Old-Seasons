#include "WPILib.h"
#include "containerGrabber.hpp"
#include "../lib/pid.hpp"

namespace frc973 {

ContainerGrabber::ContainerGrabber(Solenoid* solenoid_, CANTalon* motorA_, CANTalon* motorB_, Encoder* encoderA_, Encoder* encoderB_) {
    solenoid = solenoid_;
    motorA = motorA_;
    motorB = motorB_;
    encoderA = encoderA;
    encoderB = encoderB;

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

void ContainerGrabber::grab() {
    solenoid->Set(true);
}

void ContainerGrabber::retract() {
    solenoid->Set(false);
}

void ContainerGrabber::update() {
    switch (grabberState) {
        case DROP:
            break;
        case SETTLE:
            break;
        case PULL:
            break;
    }
}

} /* namespace frc973 */
