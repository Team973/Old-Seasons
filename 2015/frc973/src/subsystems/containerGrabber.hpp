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
        Encoder *encoder;
        bool angleFault;
        bool contact;
        bool atDriveAngle;
    };

    ContainerGrabber(CANTalon* leftMotorA_, CANTalon* leftMotorB_, CANTalon* rightMotorA_, CANTalon* rightMotorB_, Encoder *leftEncoder_, Encoder *rightEncoder_);
    void setMotorsOpenLoop(Arm* arm, float speed);
    void runArmsFreeSpeed();
    void injectArmType(int type);
    void cancelGrabSequence();
    void initGrabSequence();
    void startGrabSequence(float speed, bool teleop=false);
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
    void boost();

    const static int SLOW = 0;
    const static int DROP = 1;
    const static int SETTLE = 2;
    const static int RETRACT = 4;
    const static int HOME = 5;
    const static int TELEOP = 6;

    const static int CARBON_FIBER = 1;
    const static int ALUMINUM = 2;
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

    Timer *dropTimer;

    int armType;

    bool goinSlow;
    bool boosted;
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
