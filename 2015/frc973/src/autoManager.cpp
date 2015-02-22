#include "autoManager.hpp"
#include "stateManager.hpp"
#include "auto/commands/autoCommand.hpp"
#include "auto/commands/waitCommand.hpp"
#include "auto/commands/driveCommand.hpp"
#include "auto/commands/turnCommand.hpp"
#include "auto/commands/sauropodPathCommand.hpp"
#include "auto/commands/whipCommand.hpp"
#include "auto/commands/intakeCommand.hpp"
#include "subsystems/drive.hpp"
#include "subsystems/sauropod.hpp"

namespace frc973 {

AutoManager::AutoManager(StateManager *stateManager_)
{
    stateManager = stateManager_;
    resetModes();
}

void AutoManager::setModes() {
    modes["Test"] = new AutoSequencer();
    modes["Drive"] = new AutoSequencer();
    modes["Turn"] = new AutoSequencer();

    float driveOffset = 0.0;

    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, -1.0, false, 1));
    modes["Test"]->addConcurrent(new SauropodPathCommand(stateManager, Sauropod::READY, 2));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 0, false, 0));
    modes["Test"]->addConcurrent(new DriveCommand( .33, 2));
    modes["Test"]->addConcurrent(new WhipCommand(stateManager, "extend", 4));
    modes["Test"]->addSequential(new WhipCommand(stateManager, "retract", 0));
    modes["Test"]->addConcurrent(new DriveCommand(6.75 + driveOffset, 3));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, -1.0, false, 1));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 0.0, false, 0));
    modes["Test"]->addConcurrent(new DriveCommand( .33, 2));
    modes["Test"]->addConcurrent(new WhipCommand(stateManager, "extend", 4));
    modes["Test"]->addSequential(new WhipCommand(stateManager, "retract", 0));
    modes["Test"]->addConcurrent(new DriveCommand(6.75 + driveOffset, 4));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, -1.0, false, 1));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 0.0, false, 0));
    modes["Test"]->addSequential(new TurnCommand(90, 3));
    modes["Test"]->addSequential(new DriveCommand(new SauropodPathCommand(stateManager, Sauropod::RESTING, 2), 5, -6, 4));

    modes["Turn"]->addSequential(new TurnCommand(90, 10));

    it = modes.begin();
}

void AutoManager::resetModes() {
    modes.clear();
    setModes();
}

void AutoManager::setMode(std::string mode) {
    it = modes.find(mode);
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
