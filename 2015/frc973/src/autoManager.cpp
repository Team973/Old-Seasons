#include "autoManager.hpp"
#include "auto/autoCommand.hpp"
#include "auto/waitCommand.hpp"
#include "auto/driveCommand.hpp"
#include "auto/turnCommand.hpp"
#include "subsystems/drive.hpp"

namespace frc973 {

AutoManager::AutoManager(Drive* drive_)
{
    modes["Test"] = new AutoSequencer();
    modes["Drive"] = new AutoSequencer();

    modes["Test"]->addSequential(new WaitCommand(1));

    modes["Drive"]->addSequential(new DriveCommand(10, 10));

    it = modes.begin();
}

AutoSequencer* AutoManager::getCurrentMode()
{
    return it->second;
}

std::string AutoManager::getCurrentName()
{
    return it->first;
}

void AutoManager::nextMode()
{
    ++it;
    if (it == modes.end())
    {
        it = modes.begin();
    }
}

}
