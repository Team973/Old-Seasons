#include "WPILib.h"
#include "autoManager.hpp"

#include "auto/autoCommand.hpp"
#include "auto/autoDriveCommand.hpp"
#include "auto/sequentialCommand.hpp"
#include "auto/concurrentCommand.hpp"
#include "auto/turnCommand.hpp"
#include "auto/linearDriveCommand.hpp"
#include "auto/fireCommand.hpp"
#include "auto/armPresetCommand.hpp"
#include "auto/intakeCommand.hpp"
#include "auto/waitCommand.hpp"
#include "auto/hellavator.hpp"
#include "auto/corralCommand.hpp"
#include "kinectHandler.hpp"
#include "auto/kinectSense.hpp"
#include "auto/kinectBlock.hpp"
#include "auto/kinectJuke.hpp"
#include "auto/linearProfileCommand.hpp"
#include "auto/turnProfileCommand.hpp"
#include "auto/blockStuff.hpp"
#include <vector>
#include <math.h>

AutoManager::AutoManager(Drive *drive_, Shooter *shooter_, Intake* intake_, Arm* arm_, KinectHandler *kinect_, HellaBlocker *blocker_)
{
    drive = drive_;
    shooter = shooter_;
    intake = intake_;
    arm = arm_;
    kinect = kinect_;
    blocker = blocker_;

    distance = 0;

}

void AutoManager::inject(Timer *timer)
{
    injectTimer(timer);
}

void AutoManager::setDistance(float dist)
{
    distance = dist;
}

void AutoManager::setHeat(float hot_)
{
    hot = hot_;
}

//XXX Always put a wait at the end of auto to make sure we don't double fire
//XXX On a DriveToPoint command make the angular percision at least 5
void AutoManager::autoSelect(int autoMode)
{
    switch (autoMode)
    {
        case TEST:
            //commandSequence.push_back(new TurnProfileCommand(drive, 90, 100000, 10000, 100000, 20));
            commandSequence.push_back(new LinearProfileCommand(drive, -4, 15, 10, 15, 20));
            //commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 1));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case ONE_BALL_SIMPLE:
            commandSequence.push_back(new ArmPresetCommand(arm, CLOSE_SHOT, 0));
            commandSequence.push_back(new LinearProfileCommand(drive, 15, 15, 5, 15, 6)); 
            commandSequence.push_back(new AutoWaitCommand(1));
            commandSequence.push_back(new FireCommand(shooter, 1.5));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case NO_AUTO:
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case DRIVE_ONLY:
            commandSequence.push_back(new LinearDriveCommand(drive, 48, 0, false, 4));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case BLOCK_SIMPLE:
            commandSequence.push_back(new BlockStuff(drive, kinect, distance, B_SIMPLE, hot));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case BLOCK_90:
            commandSequence.push_back(new BlockStuff(drive, kinect, distance, B_90, hot));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case BLOCK_LOW_GOAL:
            commandSequence.push_back(new BlockStuff(drive, kinect, distance, B_LOW, hot));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case TEST_FUNCTIONAL:
            commandSequence.push_back(new LinearDriveCommand(drive, 24, 0, false, 3));
            commandSequence.push_back(new LinearDriveCommand(drive, -24, 0, false, 3));
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 1));
            commandSequence.push_back(new ArmPresetCommand(arm, PSEUDO_INTAKE, 1));
            commandSequence.push_back(new ArmPresetCommand(arm, INTAKE, 1));
            commandSequence.push_back(new ArmPresetCommand(arm, STOW, 1));
            commandSequence.push_back(new IntakeCommand(intake, arm, 4));
            commandSequence.push_back(new FireCommand(shooter, 1.5));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        default:
            break;
    }
}

void AutoManager::reset()
{
    commandSequence.clear();
}

void AutoManager::Init()
{
    AUTO_SEQUENCE = new SequentialCommand(commandSequence);
    AUTO_SEQUENCE->Init();
}

bool AutoManager::Run()
{
    if (AUTO_SEQUENCE->Run())
        return true;

    return false;
}
