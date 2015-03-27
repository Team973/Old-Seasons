#ifndef CONTAINERGRABBER_HPP
#define CONTAINERGRABBER_HPP

namespace frc973 {

class PID;

class ContainerGrabber {
    
public:
    ContainerGrabber(Solenoid* solenoid_, CANTalon* motorA_, CANTalon* motorB_, Encoder* encoderA_, Encoder* encoderB_);
    void grab();
    void retract();
    void testMotor(float speed);
    void testSetPositionTarget(float position);
    void testMotorClosedLoop(float position);
    void update();

    const static int IDLE = 0;
    const static int DROP = 1;
    const static int SETTLE = 2;
    const static int PULL = 3;
private:
    Solenoid *solenoid;
    CANTalon *motorA;
    CANTalon *motorB;
    Encoder *encoderA;
    Encoder *encoderB;

    int grabberState;

    PID *grabberPID;
};

} /* namespace frc973 */

#endif
