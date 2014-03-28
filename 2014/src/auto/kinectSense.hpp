#include "autoCommand.hpp"

#ifndef KINECT_SENSE_H
#define KINECT_SENSE_H

class KinectHandler;
class Drive;

class KinectSense : public AutoCommand
{
public:
    KinectSense(KinectHandler *kinect_, Drive *drive_, int autoMode_, float timeout_);
    virtual void Init();
    virtual bool Run();
private:
    KinectHandler *kinect;
    Drive *drive;
    AutoCommand *cmd;

    float movement;
    int autoMode;
    bool init;
};

#endif
