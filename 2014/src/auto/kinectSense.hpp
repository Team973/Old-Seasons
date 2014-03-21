#include "autoCommand.hpp"

#ifndef KINECT_SENSE_H
#define KINECT_SENSE_H

class KinectHandler;

class KinectSense : public AutoCommand
{
public:
    KinectSense(KinectHandler *kinect_);
    virtual void Init();
    virtual bool Run();
private:
    KinectHandler *kinect;
};

#endif
