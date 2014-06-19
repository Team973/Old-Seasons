#include "autoCommand.hpp"

#ifndef KINECT_BLOCK_H
#define KINECT_BLOCK_H

#define SIMPLE 1
#define HOT_SIMPLE 2
#define DOUBLE_TROUBLE 3
#define HOT_DOUBLE_TROUBLE 4
#define LOW_GOAL 5

class Drive;
class KinectHandler;
class SequentialCommand;
class HellaBlocker;

class KinectBlock : public AutoCommand
{
public:
    KinectBlock::KinectBlock(KinectHandler *kinect_, Drive *drive_, HellaBlocker *blocker_, int autoMode_, float initialDistance_, float finalDistance_, float driveTime_);
    virtual void Init();
    virtual bool Run();
private:
    Drive *drive;
    KinectHandler *kinect;
    HellaBlocker *blocker;
    int autoMode;
    float movement;
    bool goalSelected;
    float initialDistance;
    float finalDistance;
    bool init;
    float autoAngle;
    float driveTime;
    float turn;

    void clear();

    std::vector<AutoCommand*> sequence;

    SequentialCommand *cmd;
};

#endif
