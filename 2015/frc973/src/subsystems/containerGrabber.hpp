#ifndef CONTAINERGRABBER_HPP
#define CONTAINERGRABBER_HPP

namespace frc973 {

class ContainerGrabber {
    
public:
    ContainerGrabber(Solenoid* solenoid_, CANTalon* motor_);
    void grab();
    void retract();
    void testMotor(float speed);
private:
    Solenoid *solenoid;
    CANTalon *motor;
    
};

} /* namespace frc973 */

#endif
