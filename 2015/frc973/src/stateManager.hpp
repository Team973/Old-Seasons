#ifndef STATE_MANAGER_HPP
#define STATE_MANAGER_HPP

namespace frc973 {

class JoystickManager;
class Drive;

class StateManager {
public:
    StateManager(JoystickManager *controls_, Drive *drive_);
    void update();
private:
    JoystickManager *controls;
    Drive *drive;
};

}

#endif
