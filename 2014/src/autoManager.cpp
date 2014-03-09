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
#include <vector>

AutoManager::AutoManager(Drive *drive_, Shooter *shooter_, Intake* intake_, Arm* arm_, Solenoid* hellavator_)
{
    drive = drive_;
    shooter = shooter_;
    intake = intake_;
    arm = arm_;
    hellavator = hellavator_;

    driveDistance = 0;
}

void AutoManager::setHellaDistance(float dist)
{
    driveDistance = dist;
}

//XXX Always put a wait at the end of auto to make sure we don't double fire
void AutoManager::autoSelect(int autoMode)
{
    switch (autoMode)
    {
        case TEST:
            commandSequence.push_back(new TurnCommand(drive, -90, 1.5));
            commandSequence.push_back(new LinearDriveCommand(drive, 24, false, 1.5));
            commandSequence.push_back(new TurnCommand(drive, 0, 1.5));
            commandSequence.push_back(new AutoWaitCommand(2));
            commandSequence.push_back(new TurnCommand(drive, -90, 1.5));
            commandSequence.push_back(new LinearDriveCommand(drive, 24, false, 1.5));
            commandSequence.push_back(new TurnCommand(drive, 0, 1.5));
            commandSequence.push_back(new LinearDriveCommand(drive, 24, false, 1.5));
            break;
        case ONE_BALL_SIMPLE:
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 2));
            commandSequence.push_back(new FireCommand(shooter, 2));
            commandSequence.push_back(new LinearDriveCommand(drive, 60, false, 3));
            commandSequence.push_back(new AutoWaitCommand(4));
            break;
        case ONE_BALL_IN_MOVEMENT:
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 1));
            commandSequence.push_back(new LinearDriveCommand(drive, 36, false, 1.5));
            commandSequence.push_back(new FireCommand(shooter, 1.5));
            commandSequence.push_back(new LinearDriveCommand(drive, 60, false, 1));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case NO_AUTO:
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case DRIVE_ONLY:
            commandSequence.push_back(new LinearDriveCommand(drive, 96, false, 4));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case HELLAVATOR_FOREWARD:
            consecutiveSequence.push_back(new ArmPresetCommand(arm, HELLAVATOR, 1));
            consecutiveSequence.push_back(new Hellavator(hellavator, 1));
            consecutiveSequence.push_back(new LinearDriveCommand(drive, driveDistance, false, 6));
            commandSequence.push_back(new ConsecutiveCommand(consecutiveSequence));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case HELLAVATOR_BACKWARD:
            consecutiveSequence.push_back(new ArmPresetCommand(arm, HELLAVATOR, 1));
            consecutiveSequence.push_back(new Hellavator(hellavator, 1));
            consecutiveSequence.push_back(new LinearDriveCommand(drive, -driveDistance, false, 6));
            commandSequence.push_back(new ConsecutiveCommand(consecutiveSequence));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        default:
            break;
    }
}

void AutoManager::reset()
{
    commandSequence.erase(commandSequence.begin(), commandSequence.end());
    //delete AUTO_SEQUENCE;
    //AUTO_SEQUENCE = NULL;
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
