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
        bool contact;
        bool atDriveAngle;
    };

    ContainerGrabber(CANTalon* leftMotorA_, CANTalon* leftMotorB_, CANTalon* rightMotorA_, CANTalon* rightMotorB_);
    void setMotorsOpenLoop(Arm* arm, float speed);
    void runArmsFreeSpeed();
    void cancelGrabSequence();
    void initGrabSequence();
    void startGrabSequence(float speed);
    void initSettleState(Arm* arm);
    void initSlowState(Arm* arm);
    void initHomeState(Arm* arm);
    void retract();
    void setControlMode(Arm* arm, std::string mode);
    void setPIDslot(Arm* arm, int slot);
    void setPositionTarget(Arm* arm, float target);
    void setPIDTarget(float target);
    bool haveBothContact();
    bool bothAtDriveAngle();
    bool gotFault();
    bool isSettled();
    void update();

    const static int SLOW = 0;
    const static int DROP = 1;
    const static int SETTLE = 2;
    const static int RETRACT = 4;
    const static int HOME = 5;
private:

    void stateHandler(Arm* arm);
    
    CANTalon *leftMotorA;
    CANTalon *leftMotorB;
    CANTalon *rightMotorA;
    CANTalon *rightMotorB;

    Arm *leftArm;
    Arm *rightArm;

    PID *grabberPID;
    PID *homePID;

    bool goinSlow;
    float limitSpeed;
    float dropTransitionAngle;
    float dropTargetAngle;
    float settleTargetAngle;
    float angleFaultCheck;
    float driveAngle;
    float fastDriveAngle;
    float slowDriveAngle;
    float settleSpeed;
};

} /* namespace frc973 */

#endif
