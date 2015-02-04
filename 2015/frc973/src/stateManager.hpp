#ifndef STATE_MANAGER_HPP
#define STATE_MANAGER_HPP

namespace frc973 {

class Drive;

class StateManager {
public:
    StateManager(Drive *drive_);
    void setDriveFromControls(double throttle, double turn, bool quickTurn);
    void update();
private:
    Drive *drive;
};

}

#endif
