#ifndef CONTAINERGRABBER_HPP
#define CONTAINERGRABBER_HPP

namespace frc973 {

class ContainerGrabber {
    
public:
    ContainerGrabber(Solenoid* solenoid_);
    void grab();
    void retract();

private:
    Solenoid *solenoid;
    
};

} /* namespace frc973 */

#endif
