#ifndef JOYSTICK_HANDLER_HPP_
#define JOYSTICK_HANDLER_HPP_

namespace frc973 {

class JoystickManager {
public:
    JoystickManager(Joystick *coDriver_, Joystick *driver_);
    float getThrottle();
    float getTurn();
    virtual ~JoystickManager();
private:
    Joystick *coDriver;
    Joystick *driver;
};

}

#endif
