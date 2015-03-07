#include "autoManager.hpp"
#include "stateManager.hpp"
#include "auto/commands/autoCommand.hpp"
#include "auto/commands/waitCommand.hpp"
#include "auto/commands/driveCommand.hpp"
#include "auto/commands/turnCommand.hpp"
#include "auto/commands/sauropodPathCommand.hpp"
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
    modes["Pickup"]->addSequential(new SauropodPathCommand(stateManager, Sauropod::READY, 0, 15));

    float trashCan = 0.0;
    float openOffset = 4.5;
    float closedOffset = 1.0;
    float twoTote = 81.0/12.0;
    float threeTote = (twoTote*2.0);
    //float thirdFudge = 1.0;

    modes["Drive"]->addSequential(new DriveCommand(stateManager, -5.0, true, 15));

    modes["Turn"]->addSequential(new DriveCommand(stateManager, 10, true, 5));
    modes["Turn"]->addSequential(new DriveCommand(stateManager, 5, true, 5));
    modes["Turn"]->addSequential(new TurnCommand(stateManager, 90, 5));
    modes["Turn"]->addSequential(new TurnCommand(stateManager, .1, 5));

    modes["Grab"]->addSequential(new GrabCommand(stateManager, true));
    modes["Grab"]->addSequential(new WaitCommand(1.5));
    modes["Grab"]->addSequential(new DriveCommand(stateManager, 6, true, 5));
    modes["Grab"]->addSequential(new GrabCommand(stateManager, false));

    modes["ThreeTote"]->addConcurrent(new IntakeCommand(stateManager, -1.0, false, 0));
    modes["ThreeTote"]->addConcurrent(new SauropodPathCommand(stateManager, Sauropod::READY, 0, 2));
    modes["ThreeTote"]->addSequential(new IntakeCommand(stateManager, 0.0, false, 0));
    modes["ThreeTote"]->addConcurrent(new DriveCommand(stateManager, 0.0, false, 2)); // change this to trashcan
    modes["ThreeTote"]->addConcurrent(new IntakeCommand(stateManager, 1.0, false, .5));
    modes["ThreeTote"]->addSequential(new WaitCommand(0.0));
    modes["ThreeTote"]->addConcurrent(new DriveCommand(stateManager, twoTote - openOffset, true, 3));
    modes["ThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, true, 0));
    modes["ThreeTote"]->addSequential(new DriveCommand(stateManager, twoTote - closedOffset, false, 2));
    modes["ThreeTote"]->addConcurrent(new IntakeCommand(stateManager, -1.0, false, 0));
    modes["ThreeTote"]->addConcurrent(new SauropodPathCommand(stateManager, Sauropod::CONTAINER, 1, 3));
    modes["ThreeTote"]->addSequential(new SauropodPathCommand(stateManager, Sauropod::READY, 0, 2));
    //modes["ThreeTote"]->addSequential(new DriveCommand(stateManager, twoTote, false, 2));
    modes["ThreeTote"]->addSequential(new IntakeCommand(stateManager, 0.0, false, 0));
    modes["ThreeTote"]->addConcurrent(new DriveCommand(stateManager, twoTote + trashCan, false, 2));
    modes["ThreeTote"]->addConcurrent(new IntakeCommand(stateManager, 1.0, false, .5));
    modes["ThreeTote"]->addSequential(new WaitCommand(0.0));// dummy
    modes["ThreeTote"]->addConcurrent(new DriveCommand(stateManager, threeTote - openOffset, true, 3));
    modes["ThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, true, 0));
    modes["ThreeTote"]->addSequential(new DriveCommand(stateManager, threeTote - closedOffset, false, 2));
    modes["ThreeTote"]->addConcurrent(new IntakeCommand(stateManager, -1.0, false, 0));
    modes["ThreeTote"]->addConcurrent(new SauropodPathCommand(stateManager, Sauropod::CONTAINER, 1, 2));
    modes["ThreeTote"]->addSequential(new SauropodPathCommand(stateManager, Sauropod::RESTING, 0, 2));
    //modes["ThreeTote"]->addSequential(new DriveCommand(stateManager, threeTote+thirdFudge, false, 2));
    modes["ThreeTote"]->addSequential(new IntakeCommand(stateManager, 0.0, false, 0));
    modes["ThreeTote"]->addSequential(new TurnCommand(stateManager, 90, 3));
    modes["ThreeTote"]->addSequential(new DriveCommand(stateManager, -5, true, 3));
    modes["ThreeTote"]->addSequential(new IntakeCommand(stateManager, 0.0, true, 0));
    modes["ThreeTote"]->addSequential(new SauropodPathCommand(stateManager, Sauropod::CONTAINER, 3, 2));

    /*
    modes["Turn"]->addSequential(new SauropodPathCommand(stateManager, Sauropod::READY, 0, 2));
    modes["Turn"]->addConcurrent(new DriveCommand(stateManager, twoTote - openOffset, true, 3));
    modes["Turn"]->addSequential(new IntakeCommand(stateManager, -1.0, true, 0));
    modes["Turn"]->addSequential(new DriveCommand(stateManager, twoTote - closedOffset, false, 2));
    modes["Turn"]->addSequential(new IntakeCommand(stateManager, -1.0, false, 0));
    modes["Turn"]->addSequential(new DriveCommand(stateManager, twoTote, false, 2));
    modes["Turn"]->addSequential(new IntakeCommand(stateManager, 0.0, false, 0));
    modes["Turn"]->addConcurrent(new DriveCommand(stateManager, twoTote + trashCan, false, 2));
    modes["Turn"]->addConcurrent(new IntakeCommand(stateManager, 1.0, false, .5));
    modes["Turn"]->addSequential(new WaitCommand(0.0));// dummy
    modes["Turn"]->addSequential(new TurnCommand(stateManager, 90, 10));
    */

    trashCan = 0.1;
    openOffset = 4.5;
    closedOffset = 1.0;
    twoTote = 81.0/12.0;
    threeTote = (twoTote*2.0);

    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, -1.0, false, 0));
    modes["Test"]->addConcurrent(new SauropodPathCommand(stateManager, Sauropod::READY, 0, 2));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 0.0, false, 0));
    modes["Test"]->addConcurrent(new DriveCommand(stateManager, trashCan, false, 2));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, 1.0, false, .2));
    modes["Test"]->addSequential(new WaitCommand(0.0));// dummy
    modes["Test"]->addConcurrent(new TurnCommand(stateManager, 20, 3));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, 1.0, true, .5));
    modes["Test"]->addSequential(new TurnCommand(stateManager, 0, 3));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 0.0, false, 0));
    modes["Test"]->addConcurrent(new DriveCommand(stateManager, twoTote - openOffset, true, 3));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, -1.0, true, 0));
    modes["Test"]->addSequential(new DriveCommand(stateManager, twoTote - closedOffset, false, 2));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, -1.0, false, 0));
    modes["Test"]->addConcurrent(new SauropodPathCommand(stateManager, Sauropod::CONTAINER, 1, 3));
    modes["Test"]->addSequential(new SauropodPathCommand(stateManager, Sauropod::READY, 0, 2));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 0.0, false, 0));
    modes["Test"]->addConcurrent(new DriveCommand(stateManager, twoTote + trashCan, false, 2));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, 1.0, false, .5));
    modes["Test"]->addSequential(new WaitCommand(0.0));// dummy
    modes["Test"]->addConcurrent(new TurnCommand(stateManager, 20, 3));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, 1.0, true, .5));
    modes["Test"]->addSequential(new TurnCommand(stateManager, 0, 3));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 0.0, false, 0));
    modes["Test"]->addConcurrent(new DriveCommand(stateManager, threeTote - openOffset, true, 3));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, -1.0, true, 0));
    modes["Test"]->addSequential(new DriveCommand(stateManager, threeTote - closedOffset, false, 2));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, -1.0, false, 0));
    modes["Test"]->addConcurrent(new SauropodPathCommand(stateManager, Sauropod::CONTAINER, 1, 2));
    modes["Test"]->addSequential(new SauropodPathCommand(stateManager, Sauropod::RESTING, 0, 2));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 0.0, false, 0));
    modes["Test"]->addSequential(new TurnCommand(stateManager, 90, 3));
    modes["Test"]->addSequential(new DriveCommand(stateManager, -5, true, 3));
    modes["Test"]->addSequential(new IntakeCommand(stateManager, 0.0, true, 0));
    modes["Test"]->addSequential(new SauropodPathCommand(stateManager, Sauropod::CONTAINER, 3, 2));

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
