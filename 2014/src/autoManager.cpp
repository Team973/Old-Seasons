#include "WPILib.h"
#include "autoManager.hpp"

#include "auto/autoCommand.hpp"
#include "auto/autoDriveCommand.hpp"
#include "auto/sequentialCommand.hpp"
#include "auto/consecutiveCommand.hpp"
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

    initialDistance = 0;
    finalDistance = 0;
    driveTime = 0;

    side = "none";

}

void AutoManager::inject(Timer *timer)
{
    injectTimer(timer);
}

void AutoManager::setInitialDistance(float dist)
{
    initialDistance = dist*12;
}

void AutoManager::setFinalDistance(float dist)
{
    finalDistance = dist*12;
}

void AutoManager::setDriveTime(float time)
{
    driveTime = time;
}

void AutoManager::setAutoSide(std::string side_)
{
    side = side_;
}

void AutoManager::setAutoLane(std::string lane_)
{
    lane = lane_;
}

//XXX Always put a wait at the end of auto to make sure we don't double fire
//XXX On a DriveToPoint command make the angular percision at least 5
void AutoManager::autoSelect(int autoMode)
{
    switch (autoMode)
    {
        case TEST:
            /*
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 0));
            commandSequence.push_back(new CorralCommand(intake, true));
            commandSequence.push_back(new LinearDriveCommand(drive, 24, false, 1, 2));
            commandSequence.push_back(new AutoWaitCommand(1));
            commandSequence.push_back(new ArmPresetCommand(arm, INTAKE, 0.5));
            commandSequence.push_back(new CorralCommand(intake, false));
            commandSequence.push_back(new IntakeCommand(intake, arm, 2));
            commandSequence.push_back(new AutoWaitCommand(10));
            */
            //commandSequence.push_back(new AutoDriveCommand(drive, -48, 48, false, 10, 5, 8));
            //commandSequence.push_back(new AutoDriveCommand(drive, 0, 48, false, 10, 5, 8));
            //commandSequence.push_back(new TurnCommand(drive, 90, 10));
            commandSequence.push_back(new LinearDriveCommand(drive, 24, 0, false, 10));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case ONE_BALL_SIMPLE:
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 0));
            commandSequence.push_back(new LinearDriveCommand(drive, 108, 0, false, 5));
            //commandSequence.push_back(new KinectJuke(kinect, drive, .5));
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
            commandSequence.push_back(new KinectBlock(kinect, drive, blocker, SIMPLE, initialDistance, finalDistance, driveTime));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case BLOCK_HOT:
            commandSequence.push_back(new KinectBlock(kinect, drive, blocker, HOT_SIMPLE, initialDistance, finalDistance, driveTime));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case BLOCK_DOUBLE:
            commandSequence.push_back(new KinectBlock(kinect, drive, blocker, DOUBLE_TROUBLE, initialDistance, finalDistance, driveTime));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case BLOCK_DOUBLE_HOT:
            commandSequence.push_back(new KinectBlock(kinect, drive, blocker, HOT_DOUBLE_TROUBLE, initialDistance, finalDistance, driveTime));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case BLOCK_LOW_GOAL:
            commandSequence.push_back(new KinectBlock(kinect, drive, blocker, LOW_GOAL, initialDistance, finalDistance, driveTime));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case TWO_BALL:
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 0));
            commandSequence.push_back(new CorralCommand(intake, true));
            commandSequence.push_back(new LinearDriveCommand(drive, 120, 0, false, 6));
            //commandSequence.push_back(new KinectJuke(kinect, drive, 1));
            commandSequence.push_back(new FireCommand(shooter, 1));
            commandSequence.push_back(new ArmPresetCommand(arm, INTAKE, 0.5));
            commandSequence.push_back(new CorralCommand(intake, false));
            commandSequence.push_back(new IntakeCommand(intake, arm, 2));
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 1));
            //commandSequence.push_back(new KinectJuke(kinect, drive, .5));
            commandSequence.push_back(new FireCommand(shooter, 1));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case HOT_CENTER_TWO_BALL:
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 0));
            commandSequence.push_back(new CorralCommand(intake, true));
            commandSequence.push_back(new LinearDriveCommand(drive, 120, 0, false, 6));
            commandSequence.push_back(new KinectSense(kinect, drive, HOT_TWO_BALL_CENTER, 1));
            //commandSequence.push_back(new KinectJuke(kinect, drive, 1));
            commandSequence.push_back(new FireCommand(shooter, 1));
            commandSequence.push_back(new ArmPresetCommand(arm, INTAKE, 0.5));
            commandSequence.push_back(new CorralCommand(intake, false));
            commandSequence.push_back(new IntakeCommand(intake, arm, 2));
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 1));
            commandSequence.push_back(new KinectSense(kinect, drive, HOT_TWO_BALL_CENTER, 1, "none", "none", true));
            //commandSequence.push_back(new KinectJuke(kinect, drive, .5));
            commandSequence.push_back(new FireCommand(shooter, 1));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case HOT_CENTER_ONE_BALL:
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 0));
            commandSequence.push_back(new LinearDriveCommand(drive, 108, 0, false, 5));
            commandSequence.push_back(new KinectSense(kinect, drive, HOT_ONE_BALL_CENTER, 1));
            commandSequence.push_back(new FireCommand(shooter, 1.5));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case HOT_SIDE_ONE_BALL:
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 0));
            commandSequence.push_back(new LinearDriveCommand(drive, 108, 0, false, 5));
            commandSequence.push_back(new KinectSense(kinect, drive, HOT_ONE_BALL_SIDE, 1, side));
            commandSequence.push_back(new AutoWaitCommand(.5));
            //commandSequence.push_back(new KinectJuke(kinect, drive, .5));
            commandSequence.push_back(new FireCommand(shooter, 1.5));
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
