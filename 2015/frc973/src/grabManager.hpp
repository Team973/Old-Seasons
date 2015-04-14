#ifndef GRAB_MANAGER_HPP
#define GRAB_MANAGER_HPP

namespace frc973 {

class ContainerGrabber;
class Drive;

class GrabManager {
public:
    GrabManager(Drive* drive_, ContainerGrabber* grabber_);
    void runArmsFreeSpeed();
    void startSequence(float speed, bool waitForContact);
    void cancelSequence();
    void update();
private:
    Drive *drive;
    ContainerGrabber *grabber;

    bool waitForContact;
};

}

#endif
