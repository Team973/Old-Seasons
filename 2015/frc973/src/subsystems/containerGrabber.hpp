#ifndef CONTAINERGRABBER_HPP
#define CONTAINERGRABBER_HPP

namespace frc973 {

class ContainerGrabber {
    
public:
    ContainerGrabber(Solenoid* pinSolenoid_);
    void update();

private:
    Solenoid *pinSolenoid;
    
};

} /* namespace frc973 */

#endif
