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

    modes["Pickup"] = new AutoSequencer();
    modes["Pickup"]->addSequential(new SauropodPathCommand(stateManager, Sauropod::PICKUP, 2));
    modes["Pickup"]->addSequential(new SauropodPathCommand(stateManager, Sauropod::READY, 2));

    float trashCan = 0;
    float openOffset = 4.5;
    float closedOffset = 1.0;
    float twoTote = 81.0/12.0;
    float threeTote = (twoTote*2.0);
    float thirdFudge = 1.0;

    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, -1.0, false, false, 0));
    modes["Test"]->addConcurrent(new SauropodPathCommand(stateManager, Sauropod::READY, 2));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 0.0, false, false, 0));
    modes["Test"]->addConcurrent(new DriveCommand(stateManager, trashCan, false, 2));
    modes["Test"]->addConcurrent(new WhipCommand(stateManager, "extend", 4));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, 1.0, false, false, .5));
    modes["Test"]->addSequential(new WaitCommand(0.0));
    modes["Test"]->addConcurrent(new WhipCommand(stateManager, "retract", 0));
    modes["Test"]->addConcurrent(new DriveCommand(stateManager, twoTote - openOffset, true, 3));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, -1.0, true, false, 0));
    modes["Test"]->addSequential(new DriveCommand(stateManager, twoTote - closedOffset, false, 2));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, -1.0, false, false, 0));
    modes["Test"]->addSequential(new DriveCommand(stateManager, twoTote, false, 2));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 0.0, false, false, 0));
    modes["Test"]->addConcurrent(new DriveCommand(stateManager, twoTote + trashCan, false, 2));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, 1.0, false, false, .5));
    modes["Test"]->addConcurrent(new WhipCommand(stateManager, "extend", 4));
    modes["Test"]->addSequential(new WaitCommand(0.0));// dummy
    modes["Test"]->addConcurrent(new WhipCommand(stateManager, "stow", 0));
    modes["Test"]->addConcurrent(new DriveCommand(stateManager, threeTote - openOffset, true, 3));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, -1.0, true, false, 0));
    modes["Test"]->addSequential(new DriveCommand(stateManager, threeTote - closedOffset, false, 2));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, -1.0, false, true, 0));
    modes["Test"]->addSequential(new DriveCommand(stateManager, threeTote+thirdFudge, false, 2));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 0.0, false, false, 0));
    modes["Test"]->addSequential(new DriveCommand(stateManager, -3, true, 4));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 0.0, true, false, 0));
    modes["Test"]->addSequential(new SauropodPathCommand(stateManager, Sauropod::CONTAINER, 2));
    modes["Test"]->addSequential(new DriveCommand(stateManager, -2, true, 2));
    modes["Test"]->addSequential(new TurnCommand(stateManager, 0, 3));

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
