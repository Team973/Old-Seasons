#include "autoCommand.hpp"

#ifndef KINECT_JUKE_H
#define KINECT_JUKE_H

class Drive;
class KinectHandler;
class SequentialCommand;

class KinectJuke : public AutoCommand
{
public:
    KinectJuke::KinectJuke(KinectHandler *kinect_, Drive *drive_, float timeout_);
    virtual void Init();
    virtual bool Run();
    void clear();
private:

    Drive *drive;
    KinectHandler *kinect;
    std::vector<AutoCommand*> sequence;
    SequentialCommand *cmd;

    bool init;

    bool left;
    bool right;

    bool movementSelected;
};

#endif
