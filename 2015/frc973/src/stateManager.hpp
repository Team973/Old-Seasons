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
    void setSauropodState(int state);
    void setIntakeState(int state);
    bool isSauropodDone();
    void update();

    // intake states
    const static int INTAKE = 1;
    const static int EXHAUST = 2;
    const static int HUMAN_LOAD = 3;
    const static int INTAKE_IDLE = 4;
    const static int INTAKE_AUTO = 5;

    // sauropod states
    const static int LOAD = 1;
    const static int SCORE_STEP = 2;
    const static int SCORE_PLATFORM = 3;
    const static int SAUROPOD_IDLE = 4;
private:
    Drive *drive;
    Sauropod *sauropod;
    Intake *intake;

    int robotState;
    int intakeState;
    int sauropodState;

    float manualIntakeSpeed;
};

}

#endif
