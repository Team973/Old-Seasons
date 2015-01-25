#include "autoManager.hpp"
#include "auto/autoCommand.hpp"
#include "auto/waitCommand.hpp"
#include "auto/driveCommand.hpp"
#include "auto/turnCommand.hpp"
#include "subsystems/drive.hpp"

namespace frc973 {

AutoManager::AutoManager(Drive* drive_)
{
    resetModes();
}

void AutoManager::setModes() {
    modes["Test"] = new AutoSequencer();
    modes["Drive"] = new AutoSequencer();
    modes["Turn"] = new AutoSequencer();

    //modes["Test"]->addSequential(new TurnCommand(90, 10));

    modes["Drive"]->addSequential(new DriveCommand(3, 5));
    modes["Drive"]->addSequential(new TurnCommand(90, 5));
    modes["Drive"]->addSequential(new DriveCommand(3, 5));
    modes["Drive"]->addConcurrent(new TurnCommand(0, 5));
    modes["Drive"]->addConcurrent(new WaitCommand(20));
    modes["Drive"]->addSequential(new DriveCommand(3, 5));
    modes["Drive"]->addSequential(new TurnCommand(-90, 3));

    //modes["Turn"]->addSequential(new TurnCommand(90, 10));

    it = modes.begin();
}

void AutoManager::resetModes() {
    modes.clear();
    setModes();
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
