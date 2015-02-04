#ifndef STATE_MANAGER_HPP
#define STATE_MANAGER_HPP

namespace frc973 {

class Drive;
class Sauropod;

class StateManager {
public:
    StateManager(Drive *drive_, Sauropod *sauropod_);
    void setDriveFromControls(double throttle, double turn, bool quickTurn);
    void setElevatorPreset(std::string preset);
    void update();
private:
    Drive *drive;
    Sauropod *sauropod;
};

}

#endif
