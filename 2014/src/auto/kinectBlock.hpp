#include "autoCommand.hpp"

#ifndef KINECT_BLOCK_H
#define KINECT_BLOCK_H

#define SIMPLE 1
#define B_90 2
#define LOW_GOAL 3

class Drive;
class KinectHandler;
class SequentialCommand;
class HellaBlocker;

class KinectBlock : public AutoCommand
{
public:
    KinectBlock(KinectHandler *kinect_, Drive *drive_, int autoMode_, bool Hot_, float distance_);
    virtual void Init();
    virtual bool Run();
private:
    Drive *drive;
    KinectHandler *kinect;
    HellaBlocker *blocker;
    int autoMode;
    float movement;
    bool goalSelected;
    float distance;
    bool init;
    float turn;
    bool Hot;
    bool generated;
    int directionFlag;

    Timer *hotTimer;

    void clear();

    std::vector<AutoCommand*> sequence;

    SequentialCommand *cmd;
};

#endif
