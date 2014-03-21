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
#define ONE_BALL_IN_MOVEMENT 3
#define NO_AUTO 4
#define HELLAVATOR_FOREWARD 5
#define HELLAVATOR_BACKWARD 6
#define TWO_BALL 7
#define DRIVE_ONLY 8
#define TEST_FUNCTIONAL 10

class KinectHandler;

class AutoManager : public AutoCommand
{
public:
    AutoManager(Drive *drive_, Shooter* shooter_, Intake* intake_, Arm *arm_, KinectHandler *kinect_, Solenoid* hellavator_);
    void setHellaDistance(float dist);
    virtual void Init();
    virtual bool Run();
    void autoSelect(int autoMode);
    void reset();
private:
    Drive *drive;
    Shooter *shooter;
    Intake *intake;
    Arm *arm;
    KinectHandler *kinect;
    Solenoid *hellavator;
    std::vector<AutoCommand*> commandSequence;
    std::vector<AutoCommand*> consecutiveSequence;
    SequentialCommand *AUTO_SEQUENCE;
    float driveDistance;
};

#endif
