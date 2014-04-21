#ifndef KINECT_HANDLER_H
#define KINECT_HANDLER_H

class KinectHandler
{
public:
    KinectHandler(KinectStick *left_, KinectStick *right_);
    float kinectDeadband(float x, float limit);
    bool getLeftHand();
    bool getRightHand();
    std::string getScheduledHand();
    bool override(); // this is for over-riding stuff you know
    void update();
    void clearLastHand();
private:
    KinectStick *left;
    KinectStick *right;

    std::string lastHand;
};

#endif
