#ifndef STATE_MANAGER_HPP
#define STATE_MANAGER_HPP

namespace frc973 {

class ControlMap;
class Drive;

class StateManager {
public:
    StateManager(ControlMap *controls_, Drive *drive_);
    void update();
private:
    ControlMap *controls;
    Drive *drive;
};

}

#endif
