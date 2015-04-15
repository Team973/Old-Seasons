#ifndef GRAB_MANAGER_HPP
#define GRAB_MANAGER_HPP

namespace frc973 {

class ContainerGrabber;
class Drive;
class XYManager;

class GrabManager {
public:
    GrabManager(Drive* drive_, ContainerGrabber* grabber_);
    void runArmsFreeSpeed();
    void startSequence(float speed, bool waitForContact);
    void cancelSequence();
    bool isDriving();
    void update();
private:
    Drive *drive;
    ContainerGrabber *grabber;
    XYManager *xyManager;

    Timer *timer;

    bool waitForContact;
    bool goinSlow;
    bool driving;
};

}

#endif
