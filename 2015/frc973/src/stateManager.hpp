#ifndef STATE_MANAGER_HPP
#define STATE_MANAGER_HPP

namespace frc973 {

class Drive;
class Sauropod;
class Intake;

class StateManager {
public:
    StateManager(Drive *drive_, Sauropod *sauropod_, Intake *intake_);
    void vTecKickedInYo(bool kickedInYo);
    void setDriveFromControls(double throttle, double turn, bool quickTurn);
    void disableAutoStacking();
    void enableAutoStacking();
    void setIntakeSpeed(float speed);
    void setIntakePosition(bool open);
    bool gotTote();
    void dropHumanIntake(bool dropped);
    void setRobotState(int state);
    void setSauropodPath(int path);
    void setWhipPosition(std::string position);
    bool isSauropodDone();
    bool isWhipDone();
    bool isDriveLocked();
    void update();

    const static int LOAD = 1;
    const static int SCORE = 2;
    const static int IDLE = 3;
private:
    Drive *drive;
    Sauropod *sauropod;
    Intake *intake;

    Timer *lockTimer;

    bool vTec_yo;

    bool isAutoStack;

    int robotState;

    float intakeSpeed;

    int numTotes;
    bool hadTote;
};

}

#endif
