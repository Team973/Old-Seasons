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
    void setIntakePosition(bool open);
    bool gotTote();
    void setRobotState(int state);
    void setLoadReady();
    void setAutoLoadReady();
    void setLastTote(bool lastTote);
    void setContainerPickup();
    void setRestingLoad();
    bool isSauropodDone();
    bool isDriveLocked();
    void lockDrive();
    void unlockDrive();
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

    int robotState;

    std::string restingPath;
    std::string pickupPath;

    float leftIntakeSpeed;
    float rightIntakeSpeed;

    int numTotes;
    bool hadTote;
};

}

#endif
