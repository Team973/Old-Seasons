#ifndef STATE_MANAGER_HPP
#define STATE_MANAGER_HPP

namespace frc973 {

class Drive;
class Sauropod;
class Intake;
class GrabManager;

class StateManager {
public:
    StateManager(Drive *drive_, Sauropod *sauropod_, Intake *intake_, GrabManager * grabber_);
    void vTecKickedInYo(bool kickedInYo);
    void setDriveFromControls(double throttle, double turn, bool lowGear);
    void setElevatorFromControls(float speed);
    void setSauropodPreset(std::string name);
    void incrementElevatorHeight(float increment);
    void incrementCapHeight();
    void setIntakeSpeed(float speed);
    void setIntakeLeftRight(float left, float right);
    void setIntakePosition(bool actuate);
    void setFunnelPosition(bool position);
    void setLastTote();
    bool gotTote();
    void setCapState();
    void setRobotState(int state);
    void setScore();
    void setResting();
    void setContainerLoad(bool wantLoad);
    void setAutoLoad(bool wantLoad);
    void setHumanLoad(bool wantLoad);
    void setRestingLoad();
    void setRepack();
    void unBrakeClaw();
    void brakeClaw();
    void actuateClaw(bool clamp);
    bool isSauropodDone();
    bool isDriveLocked();
    void lockDrive();
    void unlockDrive();
    void update();
    void dropGrabber();
    void retractGrabber();

    const static int AUTO_LOAD = 1;
    const static int SCORE = 2;
    const static int IDLE = 3;
    const static int CONTAINER_LOAD = 4;
    const static int HUMAN_LOAD = 5;
    const static int REPACK = 6;
    const static int CAP = 7;
private:
    Drive *drive;
    Sauropod *sauropod;
    Intake *intake;
    GrabManager *grabber;

    // secondary states
    const static int RUNNING = 1;
    const static int END = 2;
    const static int DEAD = 3;

    // cap heights
    const static int ZERO = 2;
    const static int THREE = 3;
    const static int FOUR = 4;
    const static int FIVE = 5;

    Timer *lockTimer;

    bool vTec_yo;

    int robotState;
    int lastRobotState;
    int internalState;

    int capHeight;

    std::string restingPath;
    std::string pickupPath;

    float leftIntakeSpeed;
    float rightIntakeSpeed;

    bool wantContainer;
    bool wantAutoLoad;
    bool wantHumanLoad;

    bool hadTote;
    bool lastTote;
};

}

#endif
