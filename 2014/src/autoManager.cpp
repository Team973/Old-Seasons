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
#include <vector>
#include <math.h>

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
    driveDistance = dist*12;
}

//XXX Always put a wait at the end of auto to make sure we don't double fire
void AutoManager::autoSelect(int autoMode)
{
    switch (autoMode)
    {
        case TEST:
            //commandSequence.push_back(new AutoDriveCommand(drive, 0, 24, false, 2));
            commandSequence.push_back(new LinearDriveCommand(drive, 24, false, 3, 2));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case ONE_BALL_SIMPLE:
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 2));
            commandSequence.push_back(new FireCommand(shooter, 2));
            commandSequence.push_back(new LinearDriveCommand(drive, 60, false, 3));
            commandSequence.push_back(new AutoWaitCommand(4));
            break;
        case ONE_BALL_IN_MOVEMENT:
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 1));
            commandSequence.push_back(new LinearDriveCommand(drive, 48, false, 1.5));
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
            commandSequence.push_back(new Hellavator(hellavator, 0));
            commandSequence.push_back(new ArmPresetCommand(arm, HELLAVATOR, 0));
            commandSequence.push_back(new LinearDriveCommand(drive, driveDistance, false, 4));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case HELLAVATOR_BACKWARD:
            commandSequence.push_back(new Hellavator(hellavator, 0));
            commandSequence.push_back(new ArmPresetCommand(arm, HELLAVATOR, 0));
            commandSequence.push_back(new LinearDriveCommand(drive, -driveDistance, true, 4));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case TWO_BALL:
            commandSequence.push_back(new LinearDriveCommand(drive, 12, false, 1, 5));
            commandSequence.push_back(new CorralCommand(intake, true));
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 0));
            commandSequence.push_back(new LinearDriveCommand(drive, 48, false, 1.7));
            commandSequence.push_back(new FireCommand(shooter, 1));
            commandSequence.push_back(new ArmPresetCommand(arm, INTAKE, 1));
            commandSequence.push_back(new IntakeCommand(intake, arm, 2));
            commandSequence.push_back(new ArmPresetCommand(arm, SHOOTING, 1));
            commandSequence.push_back(new FireCommand(shooter, 1));
            commandSequence.push_back(new AutoWaitCommand(10));
            break;
        case TEST_FUNCTIONAL:
            commandSequence.push_back(new LinearDriveCommand(drive, 24, false, 3, 2));
            commandSequence.push_back(new LinearDriveCommand(drive, -24, false, 3, 2));
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
    delete AUTO_SEQUENCE;
    AUTO_SEQUENCE = NULL;
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
