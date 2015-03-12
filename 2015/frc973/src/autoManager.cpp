#include "autoManager.hpp"
#include "stateManager.hpp"
#include "auto/commands/autoCommand.hpp"
#include "auto/commands/waitCommand.hpp"
#include "auto/commands/driveCommand.hpp"
#include "auto/commands/turnCommand.hpp"
#include "auto/commands/sauropodCommand.hpp"
#include "auto/commands/intakeCommand.hpp"
#include "auto/commands/grabCommand.hpp"
#include "subsystems/drive.hpp"
#include "subsystems/sauropod.hpp"

namespace frc973 {

AutoManager::AutoManager(StateManager *stateManager_)
{
    stateManager = stateManager_;
    resetModes();
}

void AutoManager::setModes() {
    modes["ThreeTote"] = new AutoSequencer();
    modes["Drive"] = new AutoSequencer();
    modes["Turn"] = new AutoSequencer();
    modes["Grab"] = new AutoSequencer();
    modes["Test"] = new AutoSequencer();

    modes["Pickup"] = new AutoSequencer();
    modes["Pickup"]->addSequential(new SauropodCommand(stateManager, "loadHigh", 0, 15));

    float openOffset = 2.5;
    float closedOffset = 1.0;
    float twoTote = 81.0/12.0;
    float threeTote = (twoTote*2.0);
    //float thirdFudge = 1.0;

    modes["Drive"]->addSequential(new DriveCommand(stateManager, -5.0, true));

    modes["Turn"]->addSequential(new DriveCommand(stateManager, 10, true));
    modes["Turn"]->addSequential(new DriveCommand(stateManager, 5, true));
    modes["Turn"]->addSequential(new TurnCommand(stateManager, 90, 5));
    modes["Turn"]->addSequential(new TurnCommand(stateManager, .1, 5));

    modes["Grab"]->addSequential(new GrabCommand(stateManager, true));
    modes["Grab"]->addSequential(new WaitCommand(1.5));
    modes["Grab"]->addSequential(new DriveCommand(stateManager, 6, true));
    modes["Grab"]->addSequential(new GrabCommand(stateManager, false));

    modes["ThreeTote"]->addConcurrent(new SauropodCommand(stateManager, "loadHigh", 0, 0));
    modes["ThreeTote"]->addConcurrent(new IntakeCommand(stateManager, 1.0, "float", 0));
    modes["ThreeTote"]->addConcurrent(new DriveCommand(stateManager, twoTote - openOffset, false));
    modes["ThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, "open", 0));
    modes["ThreeTote"]->addSequential(new DriveCommand(stateManager, twoTote - closedOffset, true));
    modes["ThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, "float", 0));
    modes["ThreeTote"]->addSequential(new SauropodCommand(stateManager, "loadLow", 0.5, 2));
    modes["ThreeTote"]->addConcurrent(new SauropodCommand(stateManager, "loadHigh", 0, 0));
    modes["ThreeTote"]->addConcurrent(new IntakeCommand(stateManager, 1.0, "float", 0));
    modes["ThreeTote"]->addConcurrent(new DriveCommand(stateManager, threeTote - openOffset, false));
    modes["ThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, "open", 0));
    modes["ThreeTote"]->addSequential(new DriveCommand(stateManager, threeTote - closedOffset, true));
    modes["ThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, "float", 0));
    modes["ThreeTote"]->addSequential(new SauropodCommand(stateManager, "loadLow", 0.5, 2));
    modes["ThreeTote"]->addConcurrent(new IntakeCommand(stateManager, 0.0, "closed", 0));
    modes["ThreeTote"]->addConcurrent(new SauropodCommand(stateManager, "rest", 0, 1));
    modes["ThreeTote"]->addSequential(new TurnCommand(stateManager, 90, 5));
    modes["ThreeTote"]->addSequential(new DriveCommand(stateManager, twoTote + closedOffset, true));
    modes["ThreeTote"]->addSequential(new IntakeCommand(stateManager, 0.0, "open", 0));
    modes["ThreeTote"]->addSequential(new DriveCommand(stateManager, twoTote - closedOffset, true));

    openOffset = .5;
    closedOffset = .25;
    twoTote = 1.0;
    threeTote = (twoTote*2.0);

    modes["Test"]->addConcurrent(new SauropodCommand(stateManager, "loadHigh", 0, 0));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, 1.0, "float", 0));
    modes["Test"]->addConcurrent(new DriveCommand(stateManager, twoTote - openOffset, false));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, -1.0, "open", 0));
    modes["Test"]->addSequential(new DriveCommand(stateManager, twoTote - closedOffset, true));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, -1.0, "float", 0));
    modes["Test"]->addSequential(new SauropodCommand(stateManager, "loadLow", 0.5, 2));
    modes["Test"]->addConcurrent(new SauropodCommand(stateManager, "loadHigh", 0, 0));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, 1.0, "float", 0));
    modes["Test"]->addConcurrent(new DriveCommand(stateManager, threeTote - openOffset, false));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, -1.0, "open", 0));
    modes["Test"]->addSequential(new DriveCommand(stateManager, threeTote - closedOffset, true));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, -1.0, "float", 0));
    modes["Test"]->addSequential(new SauropodCommand(stateManager, "loadLow", 0.5, 2));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, 0.0, "closed", 0));
    modes["Test"]->addConcurrent(new SauropodCommand(stateManager, "rest", 0, 1));
    modes["Test"]->addSequential(new TurnCommand(stateManager, 90, 5));
    modes["Test"]->addSequential(new DriveCommand(stateManager, twoTote + closedOffset, true));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 0.0, "open", 0));
    modes["Test"]->addSequential(new DriveCommand(stateManager, twoTote - closedOffset, true));

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
