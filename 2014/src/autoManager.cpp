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
#include <vector>
#include <math.h>

AutoManager::AutoManager(Drive *drive_, Shooter *shooter_, Intake* intake_, Arm* arm_, KinectHandler *kinect_, Solenoid* hellavator_)
{
    drive = drive_;
    shooter = shooter_;
    intake = intake_;
    arm = arm_;
    kinect = kinect_;
    hellavator = hellavator_;

    driveDistance = 0;


}

void AutoManager::inject(Timer *timer)
{
    injectTimer(timer);
}

void AutoManager::setHellaDistance(float dist)
{
    driveDistance = dist*12;
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
            commandSequence.push_back(new TurnCommand(drive, 90, 10));
            //commandSequence.push_back(new LinearDriveCommand(drive, 120, -5, false, 5));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case ONE_BALL_SIMPLE:
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 0));
            commandSequence.push_back(new LinearDriveCommand(drive, 108, 0, false, 5));
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
        case HELLAVATOR_FOREWARD:
            commandSequence.push_back(new Hellavator(hellavator, 0));
            commandSequence.push_back(new ArmPresetCommand(arm, HELLAVATOR, 0));
            commandSequence.push_back(new LinearDriveCommand(drive, driveDistance, 0, false, 4));
            commandSequence.push_back(new KinectSense(kinect, drive, BLOCKER, false, 10));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case HELLAVATOR_BACKWARD:
            commandSequence.push_back(new Hellavator(hellavator, 0));
            commandSequence.push_back(new ArmPresetCommand(arm, HELLAVATOR, 0));
            commandSequence.push_back(new LinearDriveCommand(drive, -driveDistance, 0, true, 4));
            commandSequence.push_back(new KinectSense(kinect, drive, BLOCKER, false, 10));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case TWO_BALL:
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 0));
            commandSequence.push_back(new CorralCommand(intake, true));
            commandSequence.push_back(new LinearDriveCommand(drive, 144, 0, false, 6));
            commandSequence.push_back(new AutoWaitCommand(1));
            commandSequence.push_back(new FireCommand(shooter, 1));
            commandSequence.push_back(new ArmPresetCommand(arm, INTAKE, 0.5));
            commandSequence.push_back(new CorralCommand(intake, false));
            commandSequence.push_back(new IntakeCommand(intake, arm, 2));
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 1));
            commandSequence.push_back(new AutoWaitCommand(.5));
            commandSequence.push_back(new FireCommand(shooter, 1));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case TWO_LEFT:
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 0));
            commandSequence.push_back(new CorralCommand(intake, true));
            commandSequence.push_back(new LinearDriveCommand(drive, 144, 0, false, 6));
            commandSequence.push_back(new AutoWaitCommand(1));
            commandSequence.push_back(new FireCommand(shooter, 1));
            commandSequence.push_back(new ArmPresetCommand(arm, INTAKE, 0.5));
            commandSequence.push_back(new CorralCommand(intake, false));
            commandSequence.push_back(new IntakeCommand(intake, arm, 2));
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 1));
            commandSequence.push_back(new AutoWaitCommand(.5));
            commandSequence.push_back(new FireCommand(shooter, 1));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case TWO_RIGHT:
            break;
        case TEST_FUNCTIONAL:
            commandSequence.push_back(new LinearDriveCommand(drive, 24, 0, false, 3, 2));
            commandSequence.push_back(new LinearDriveCommand(drive, -24, 0, false, 3, 2));
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
