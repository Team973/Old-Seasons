#include "autoCommand.hpp"

#ifndef KINECT_JUKE_H
#define KINECT_JUKE_H

class Drive;
class KinectHandler;

class KinectJuke : public AutoCommand
{
public:
    KinectJuke(KinectHandler *kinect_, Drive *drive_, float timeout_);
    virtual void Init();
    virtual bool Run();
private:
    KinectHandler *kinect;
    Drive *drive;
    AutoCommand *cmd;

    bool init;
};

#endif
