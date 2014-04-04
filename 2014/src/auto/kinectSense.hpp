#include "autoCommand.hpp"

#ifndef KINECT_SENSE_H
#define KINECT_SENSE_H

#define MULTI_BALL 1
#define BLOCKER 2

class KinectHandler;
class Drive;
class SequentialCommand;

class KinectSense : public AutoCommand
{
public:
    KinectSense(KinectHandler *kinect_, Drive *drive_, int autoMode_, bool instantExecution_, float timeout_);
    virtual void Init();
    virtual bool Run();
private:
    void clear();


    KinectHandler *kinect;
    Drive *drive;
    std::vector<AutoCommand*> sequence;
    SequentialCommand *cmd;

    float movement;
    int autoMode;
    bool init;
    bool instantExecution;
};

#endif
