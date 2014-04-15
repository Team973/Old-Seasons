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
private:
    KinectStick *left;
    KinectStick *right;

    std::string lastHand;
};

#endif
