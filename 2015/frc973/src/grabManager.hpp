#ifndef GRAB_MANAGER_HPP
#define GRAB_MANAGER_HPP

namespace frc973 {

class ContainerGrabber;
class Drive;
class XYManager;

class GrabManager {
public:
    GrabManager(Drive* drive_, Talon* leftArm_, Talon* rightArm_);
    void runArms();
    void init();
    void cancelSequence();
    void update();
private:
    Drive *drive;

    Timer *timer;

    Talon *leftArm;
    Talon *rightArm;

    bool sequenceCanceled;
};

}

#endif
