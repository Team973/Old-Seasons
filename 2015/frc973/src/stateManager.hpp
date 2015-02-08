#ifndef STATE_MANAGER_HPP
#define STATE_MANAGER_HPP

namespace frc973 {

class Drive;
class Sauropod;
class Intake;

class StateManager {
public:
    StateManager(Drive *drive_, Sauropod *sauropod_, Intake *intake_);
    void setDriveFromControls(double throttle, double turn, bool quickTurn);
    void setIntakeFromControls(float manual);
    void setSauropodPreset(std::string preset);
    void update();
private:
    Drive *drive;
    Sauropod *sauropod;
    Intake *intake;

    float manualIntakeSpeed;
};

}

#endif
