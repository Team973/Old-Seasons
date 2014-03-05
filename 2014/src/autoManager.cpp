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
#include <vector>

AutoManager::AutoManager(Drive *drive_, Shooter *shooter_, Intake* intake_, Arm* arm_)
{
    drive = drive_;
    shooter = shooter_;
    intake = intake_;
    arm = arm_;
}

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
            commandSequence.push_back(new LinearDriveCommand(drive, 60, false, 5));
            break;
        default:
            break;
    }
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
