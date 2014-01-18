#include "WPILib.h"
#include "autoManager.hpp"

#include "auto/autoCommand.hpp"
#include "auto/autoDriveCommand.hpp"
#include "auto/sequentialCommand.hpp"
#include "auto/consecutiveCommand.hpp"
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
            commandSequence.push_back(new AutoDriveCommand(drive, 24, 0, false, 0.5));
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
