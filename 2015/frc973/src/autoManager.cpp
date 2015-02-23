#include "autoManager.hpp"
#include "stateManager.hpp"
#include "auto/commands/autoCommand.hpp"
#include "auto/commands/waitCommand.hpp"
#include "auto/commands/driveCommand.hpp"
#include "auto/commands/turnCommand.hpp"
#include "auto/commands/sauropodPathCommand.hpp"
#include "auto/commands/whipCommand.hpp"
#include "auto/commands/intakeCommand.hpp"
#include "auto/commands/waitForToteCommand.hpp"
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

    modes["Pickup"] = new AutoSequencer();
    modes["Pickup"]->addSequential(new SauropodPathCommand(stateManager, Sauropod::PICKUP, 2));
    modes["Pickup"]->addSequential(new SauropodPathCommand(stateManager, Sauropod::READY, 2));

    float driveOffset = 0.0;

    //modes["Test"]->addSequential(new IntakeCommand(stateManager, -1.0, true, 10));

    // tote one
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, -1.0, false, 1));
    modes["Test"]->addConcurrent(new SauropodPathCommand(stateManager, Sauropod::READY, 2));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 1.0, false, 0));
    modes["Test"]->addConcurrent(new DriveCommand(stateManager, .33, false, 2));
    modes["Test"]->addConcurrent(new WhipCommand(stateManager, "extend", 4));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 0, false, 0));
    modes["Test"]->addSequential(new WhipCommand(stateManager, "retract", 0));
    // tote two
    modes["Test"]->addConcurrent(new DriveCommand(stateManager, 5.5 + driveOffset, true, 0));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, -1.0, true, 0));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, -1.0, false, 0));
    //modes["Test"]->addSequential(new DriveCommand(stateManager, new IntakeCommand(stateManager, -1.0, false, 0), 1.9, 2, false, 3));
    modes["Test"]->addSequential(new DriveCommand(stateManager, 2, false, 3));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 0.0, false, 0));
    modes["Test"]->addConcurrent(new DriveCommand(stateManager, .33, false, 2));
    modes["Test"]->addConcurrent(new WhipCommand(stateManager, "extend", 4));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, 1.0, false, 3));
    modes["Test"]->addSequential(new WhipCommand(stateManager, "retract", 0));
    // tote three
    modes["Test"]->addConcurrent(new DriveCommand(stateManager, 5.5 + driveOffset, true, 3));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, -1.0, true, 0));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, -1.0, false, 0));
    modes["Test"]->addSequential(new DriveCommand(stateManager, new IntakeCommand(stateManager, -1.0, false, 0), 1.9, 2, false, 3));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 0.0, false, 0));
    /*
    modes["Test"]->addSequential(new TurnCommand(90, 3));
    modes["Test"]->addSequential(new DriveCommand(new SauropodPathCommand(stateManager, Sauropod::RESTING, 2), 5, -6, 4));
    */

    modes["Turn"]->addSequential(new TurnCommand(stateManager, 90, 10));

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
