#include "autoManager.hpp"
#include "auto/autoCommand.hpp"
#include "auto/waitCommand.hpp"
#include "auto/driveCommand.hpp"
#include "auto/turnCommand.hpp"
#include "subsystems/drive.hpp"

namespace frc973 {

AutoManager::AutoManager(Drive* drive_)
{
    currMode = 0;

    modes.push_back(new AutoSequencer("Test"));
    modes.back()->addSequential(new WaitCommand(1));

    modes.push_back(new AutoSequencer("Drive"));
    modes.back()->addSequential(new TurnCommand(90, 10));
    //modes.back()->addSequential(new DriveCommand(10, 10));
}

AutoSequencer* AutoManager::getCurrentMode()
{
    return modes[currMode];
}

std::string AutoManager::getCurrentName()
{
    return modes[currMode]->getName();
}

void AutoManager::nextMode()
{
    currMode += 1;
    if (currMode > modes.size()-1)
    {
        currMode -= modes.size();
    }
    else if (currMode < modes.size()-1)
    {
        currMode += modes.size();
    }
}

}
