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
    void setRobotState(int state);
    void setSauropodPath(int path);
    void setWhipPosition(float position);
    bool isSauropodDone();
    void update();

    const static int LOAD = 1;
    const static int SCORE = 2;
    const static int IDLE = 3;
private:
    Drive *drive;
    Sauropod *sauropod;
    Intake *intake;

    int robotState;

    bool fromControls;

    float manualIntakeSpeed;
};

}

#endif
