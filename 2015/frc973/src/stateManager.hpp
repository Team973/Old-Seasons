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
    void incrementElevatorHeight(float increment);
    void disableAutoStacking();
    void enableAutoStacking();
    void setIntakeSpeed(float speed);
    void setIntakePosition(bool open);
    bool gotTote();
    void dropHumanIntake(bool dropped);
    void setRobotState(int state);
    void setSauropodPath(int path);
    void setLoadReady();
    void setAutoLoadReady();
    void setLastTote(bool lastTote);
    void autoSetLastTote();
    void setContainerPickup();
    void setRestingLoad();
    void setWhipPosition(std::string position);
    bool isSauropodDone();
    bool isWhipDone();
    bool isDriveLocked();
    void lockDrive();
    void unlockDrive();
    void lockWhip();
    void unlockWhip();
    bool isWhipLocked();
    void update();

    const static int LOAD = 1;
    const static int SCORE = 2;
    const static int IDLE = 3;
private:
    Drive *drive;
    Sauropod *sauropod;
    Intake *intake;
    ContainerGrabber *grabber;

    Timer *lockTimer;

    bool vTec_yo;

    bool isAutoStack;

    int robotState;

    int restingPath;
    int pickupPath;

    float intakeSpeed;

    int numTotes;
    bool hadTote;
};

}

#endif
