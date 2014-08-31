#include "autoCommand.hpp"

#ifndef BLOCK_STUFF_H
#define BLOCK_STUFF_H

#define B_SIMPLE 1
#define B_90 2
#define B_LOW 3

class Drive;
class KinectHandler;
class SequentialCommand;
class PID;

class BlockStuff : public AutoCommand
{
public:
    BlockStuff(Drive *drive_, KinectHandler *kinect_, float distance_, int mode_, bool hot_);
    virtual void Init();
    virtual bool Run();
private:
    Drive *drive;
    KinectHandler *kinect;
    float distance;
    int mode;
    bool hot;

    PID *holdAnglePID;

    Timer *hotTimer;

    bool generated;
    bool directionSelected;
    bool kinectOver;
    int directionFlag;

    float angle;

    bool init;

    SequentialCommand *cmd;
    std::vector<AutoCommand*> sequence;
};

#endif
