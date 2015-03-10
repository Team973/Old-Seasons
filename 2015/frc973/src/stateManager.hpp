#ifndef STATE_MANAGER_HPP
#define STATE_MANAGER_HPP

namespace frc973 {

class Drive;
class Sauropod;
class Intake;
class ContainerGrabber;

class StateManager {
public:
    StateManager(Drive *drive_, Sauropod *sauropod_, Intake *intake_, ContainerGrabber *grabber_);
    void vTecKickedInYo(bool kickedInYo);
    void setDriveFromControls(double throttle, double turn, bool lowGear);
    void setElevatorFromControls(float speed);
    void dropGrabber();
    void raiseGrabber();
    void setSauropodPreset(std::string name);
    void incrementElevatorHeight(float increment);
    void setIntakeSpeed(float speed);
    void setIntakeLeftRight(float left, float right);
    void setIntakePosition(std::string position);
    bool gotTote();
    void setRobotState(int state);
    void setScore();
    void setHigh();
    void setContainerLoad(bool wantLoad);
    void setAutoLoad(bool wantLoad);
    void setHumanLoad(bool wantLoad);
    void setRestingLoad();
    bool isSauropodDone();
    bool isDriveLocked();
    void lockDrive();
    void unlockDrive();
    void update();

    const static int AUTO_LOAD = 1;
    const static int SCORE = 2;
    const static int IDLE = 3;
    const static int CONTAINER_LOAD = 4;
    const static int HUMAN_LOAD = 5;
private:
    Drive *drive;
    Sauropod *sauropod;
    Intake *intake;
    ContainerGrabber *grabber;

    // secondary states
    const static int RUNNING = 1;
    const static int END = 2;
    const static int DEAD = 3;

    Timer *lockTimer;

    bool vTec_yo;

    int robotState;
    int lastRobotState;
    int internalState;

    std::string restingPath;
    std::string pickupPath;

    float leftIntakeSpeed;
    float rightIntakeSpeed;

    bool wantContainer;
    bool wantAutoLoad;
    bool wantHumanLoad;

    int numTotes;
    bool hadTote;
};

}

#endif
