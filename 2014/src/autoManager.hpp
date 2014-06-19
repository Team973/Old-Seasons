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
#define BLOCK_HOT 5
#define BLOCK_DOUBLE 6
#define BLOCK_DOUBLE_HOT 7
#define BLOCK_LOW_GOAL 8
#define TWO_BALL 9
#define HOT_CENTER_TWO_BALL 10
#define HOT_CENTER_ONE_BALL 11
#define HOT_SIDE_ONE_BALL 12
#define DRIVE_ONLY 13
#define TEST_FUNCTIONAL 42

class KinectHandler;
class HellaBlocker;

class AutoManager : public AutoCommand
{
public:
    AutoManager(Drive *drive_, Shooter* shooter_, Intake* intake_, Arm *arm_, KinectHandler *kinect_, HellaBlocker *blocker_);
    void setInitialDistance(float dist);
    void setFinalDistance(float dist);
    void setDriveTime(float time);
    void setAutoSide(std::string side_);
    void setAutoLane(std::string lane_);
    virtual void Init();
    virtual bool Run();
    void autoSelect(int autoMode);
    void reset();
    void inject(Timer *timer);
private:
    Drive *drive;
    Shooter *shooter;
    Intake *intake;
    Arm *arm;
    KinectHandler *kinect;
    HellaBlocker *blocker;
    std::vector<AutoCommand*> commandSequence;
    std::vector<AutoCommand*> consecutiveSequence;
    SequentialCommand *AUTO_SEQUENCE;
    float initialDistance;
    float finalDistance;
    float driveTime;
    std::string side;
    std::string lane;
};

#endif
