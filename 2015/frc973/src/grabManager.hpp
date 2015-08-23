#ifndef GRAB_MANAGER_HPP
#define GRAB_MANAGER_HPP

namespace frc973 {

class GrabManager {
public:
    GrabManager(Solenoid* grabber_);
    void drop();
    void retract();
private:
    Solenoid *grabber;
};

}

#endif
