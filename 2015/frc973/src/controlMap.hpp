#ifndef CONTROL_MAP_HPP_
#define CONTROL_MAP_HPP_

namespace frc973 {

class ControlMap {
public:
    ControlMap(Joystick *coDriver_, Joystick *driver_);
    float getThrottle();
    float getTurn();
    bool getLowGear();
    bool getCodriverButton(int button);
    float getCodriverAxis(int axis);
    bool getDriverButton(int button);
    float getDriverAxis(int axis);
    bool getCodriverDpadDown();
    bool getCodriverDpadUp();
private:
    Joystick *coDriver;
    Joystick *driver;

    float prevCodriverDpad;
};

}

#endif
