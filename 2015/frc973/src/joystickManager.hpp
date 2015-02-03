#ifndef JOYSTICK_MANAGER_HPP_
#define JOYSTICK_MANAGER_HPP_

namespace frc973 {

class JoystickManager {
public:
    JoystickManager(Joystick *coDriver_, Joystick *driver_);
    float getThrottle();
    float getTurn();
private:
    Joystick *coDriver;
    Joystick *driver;
};

}

#endif
