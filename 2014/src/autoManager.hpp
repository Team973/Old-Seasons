#include "WPILib.h"
#include "auto/autoCommand.hpp"
#include "auto/sequentialCommand.hpp"
#include <vector>
#include "drive.hpp"
#include "shooter.hpp"
#include "intake.hpp"
#include "arm.hpp"

#ifndef AUTO_MANAGER_H
#define AUTO_MANAGER_H

#define TEST 1
#define ONE_BALL_SIMPLE 2
#define NO_AUTO 3
#define BLOCK_SIMPLE 4
#define BLOCK_90 5
#define BLOCK_LOW_GOAL 6
#define DRIVE_ONLY 7
#define TEST_FUNCTIONAL 42

class KinectHandler;
class HellaBlocker;

class AutoManager : public AutoCommand
{
public:
    AutoManager(Drive *drive_, Shooter* shooter_, Intake* intake_, Arm *arm_, KinectHandler *kinect_, HellaBlocker *blocker_);
    void setDistance(float dist);
    virtual void Init();
    virtual bool Run();
    void autoSelect(int autoMode);
    void reset();
    void inject(Timer *timer);
    void setHeat(float hot_);
private:
    Drive *drive;
    Shooter *shooter;
    Intake *intake;
    Arm *arm;
    KinectHandler *kinect;
    HellaBlocker *blocker;
    std::vector<AutoCommand*> commandSequence;
    std::vector<AutoCommand*> concurrentSequence;
    SequentialCommand *AUTO_SEQUENCE;
    float distance;
    bool hot;
};

#endif
