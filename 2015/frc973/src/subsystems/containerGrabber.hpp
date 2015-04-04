#ifndef CONTAINERGRABBER_HPP
#define CONTAINERGRABBER_HPP

namespace frc973 {

class PID;

class ContainerGrabber {

public:

    struct Arm {
        int state;
        CANTalon *motorA;
        CANTalon *motorB;
        bool angleFault;
        Timer *timer;
    };

    ContainerGrabber(CANTalon* leftMotorA_, CANTalon* leftMotorB_, CANTalon* rightMotorA_, CANTalon* rightMotorB_);
    void initGrabSequence();
    void startGrabSequence();
    void initSettleState(Arm arm);
    void initPullState(Arm arm);
    void initIdleState(Arm arm);
    void retract();
    void testMotor(float speed);
    void testSetPositionTarget(float position);
    void testMotorClosedLoop();
    ContainerGrabber::Arm testGetArm(int arm);
    void setControlMode(Arm arm, std::string mode);
    void setPIDslot(Arm arm, int slot);
    void setPositionTarget(Arm arm, float target);
    void setPIDTarget(float target);
    void update();

    const static int IDLE = 0;
    const static int DROP = 1;
    const static int SETTLE = 2;
    const static int PULL = 3;
    const static int RETRACT = 4;
private:

    void stateHandler(Arm arm);
    
    CANTalon *leftMotorA;
    CANTalon *leftMotorB;
    CANTalon *rightMotorA;
    CANTalon *rightMotorB;

    Arm leftArm;
    Arm rightArm;

    PID *grabberPID;
};

} /* namespace frc973 */

#endif
