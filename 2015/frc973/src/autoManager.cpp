#include "autoManager.hpp"
#include "stateManager.hpp"
#include "auto/commands/autoCommand.hpp"
#include "auto/commands/waitCommand.hpp"
#include "auto/commands/driveCommand.hpp"
#include "auto/commands/turnCommand.hpp"
#include "auto/commands/sauropodCommand.hpp"
#include "auto/commands/intakeCommand.hpp"
#include "auto/commands/clawCommand.hpp"
#include "subsystems/drive.hpp"
#include "subsystems/sauropod.hpp"
#include "lib/socketClient.hpp"

namespace frc973 {

AutoManager::AutoManager(StateManager *stateManager_)
{
    stateManager = stateManager_;
    resetModes();
    SocketClient::AddListener("autoMode", this);
}

void AutoManager::setModes() {
    modes["BasicThreeTote"] = new AutoSequencer();
    modes["TurnThreeTote"] = new AutoSequencer();
    modes["Clamp"] = new AutoSequencer();
    modes["Drive"] = new AutoSequencer();
    modes["Turn"] = new AutoSequencer();
    modes["Test"] = new AutoSequencer();
    modes["None"] = new AutoSequencer();

    modes["Pickup"] = new AutoSequencer();
    modes["Pickup"]->addSequential(new SauropodCommand(stateManager, "loadHigh", 0, false, 15));

    float openOffset = 2.5;
    float closedOffset = 1.0;
    float twoTote = 81.0/12.0;
    float threeTote = (twoTote*2.0);
    //float dropOffset = 1;
    //float driveOffset = 4.0;
    //float thirdFudge = 1.0;

    modes["Drive"]->addSequential(new DriveCommand(stateManager, -5.0, "fast"));

    modes["Turn"]->addSequential(new TurnCommand(stateManager, -180, 1.0, 15));

    modes["Clamp"]->addSequential(new ClawCommand(stateManager, true));

    modes["None"]->addSequential(new WaitCommand(10000));


    modes["BasicThreeTote"]->addConcurrent(new SauropodCommand(stateManager, "humanLoadHigh", 0, false, 0));
    modes["BasicThreeTote"]->addConcurrent(new IntakeCommand(stateManager, 1.0, false, 0));
    modes["BasicThreeTote"]->addSequential(new DriveCommand(stateManager, twoTote - openOffset, "slow"));
    modes["BasicThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, true, 0));
    modes["BasicThreeTote"]->addConcurrent(new SauropodCommand(stateManager, "loadHigh", 0, false, 0));
    modes["BasicThreeTote"]->addConcurrent(new DriveCommand(stateManager, twoTote - closedOffset - 0.5, "hellaFast"));
    modes["BasicThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, false, 0));
    modes["BasicThreeTote"]->addSequential(new SauropodCommand(stateManager, "loadLow", 0.5, false, 2));
    modes["BasicThreeTote"]->addConcurrent(new SauropodCommand(stateManager, "humanLoadHigh", 0, false, 0));
    modes["BasicThreeTote"]->addConcurrent(new IntakeCommand(stateManager, 1.0, false, 0));
    modes["BasicThreeTote"]->addConcurrent(new DriveCommand(stateManager, threeTote - openOffset - 1.0, "slow"));
    modes["BasicThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, true, 0));
    modes["BasicThreeTote"]->addConcurrent(new SauropodCommand(stateManager, "loadHigh", 0, false, 0));
    modes["BasicThreeTote"]->addConcurrent(new DriveCommand(stateManager, threeTote - closedOffset + 0.5, "hellaFast"));
    modes["BasicThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, false, 0));
    modes["BasicThreeTote"]->addSequential(new SauropodCommand(stateManager, "loadLow", 0.5, false, 2));
    modes["BasicThreeTote"]->addSequential(new SauropodCommand(stateManager, "rest", 0, true, 1));
    modes["BasicThreeTote"]->addConcurrent(new TurnCommand(stateManager, -50, 0.9, 5));
    modes["BasicThreeTote"]->addConcurrent(new IntakeCommand(stateManager, 0.0, false, 0));
    modes["BasicThreeTote"]->addSequential(new DriveCommand(stateManager, threeTote + 8.5, "hellaFast"));
    modes["BasicThreeTote"]->addConcurrent(new TurnCommand(stateManager, -50, 0.9, 5));
    modes["BasicThreeTote"]->addConcurrent(new SauropodCommand(stateManager, "autoScore", 0.0, true, 0, true));
    modes["BasicThreeTote"]->addConcurrent(new IntakeCommand(stateManager, 0.0, true, 0));
    modes["BasicThreeTote"]->addSequential(new DriveCommand(stateManager, threeTote, "hellaFast"));

    float driveBack = 2.0;
    float driveBackOffset = 1.0;
    openOffset = 2.5;
    closedOffset = 1.0;
    twoTote = (81.0/12.0) + driveBack + driveBackOffset;
    threeTote = (twoTote*2.0) - driveBack - driveBackOffset;

    modes["TurnThreeTote"]->addSequential(new ClawCommand(stateManager, true));
    modes["TurnThreeTote"]->addSequential(new SauropodCommand(stateManager, "loadHigh", 0, false, 0));
    modes["TurnThreeTote"]->addSequential(new WaitCommand(0.25));
    modes["TurnThreeTote"]->addConcurrent(new IntakeCommand(stateManager, -1.0, false, 0));
    modes["TurnThreeTote"]->addConcurrent(new DriveCommand(stateManager, driveBack, "fast"));
    modes["TurnThreeTote"]->addSequential(new SauropodCommand(stateManager, "loadLow", 0, false, 2));
    modes["TurnThreeTote"]->addConcurrent(new SauropodCommand(stateManager, "loadHigh", 0, false, 0));
    modes["TurnThreeTote"]->addConcurrent(new TurnCommand(stateManager, 173, 0.6, 5));
    modes["TurnThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, true, 0));
    modes["TurnThreeTote"]->addSequential(new WaitCommand(0.25));
    modes["TurnThreeTote"]->addSequential(new DriveCommand(stateManager, twoTote - closedOffset - 0.5, "hellaFast"));
    modes["TurnThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, false, 0));
    modes["TurnThreeTote"]->addSequential(new SauropodCommand(stateManager, "loadLow", 0.75, false, 2));
    //modes["TurnThreeTote"]->addConcurrent(new SauropodCommand(stateManager, "humanLoadHigh", 0, false, 0));
    modes["TurnThreeTote"]->addConcurrent(new SauropodCommand(stateManager, "repack", 0.0, true, 0, true));
    //modes["TurnThreeTote"]->addConcurrent(new TurnCommand(stateManager, 173, 0.6, 3)); correction turn
    modes["TurnThreeTote"]->addConcurrent(new IntakeCommand(stateManager, 1.0, false, 0));
    modes["TurnThreeTote"]->addConcurrent(new DriveCommand(stateManager, threeTote - openOffset - 0.5, "slow"));
    modes["TurnThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, true, 0));
    modes["TurnThreeTote"]->addConcurrent(new SauropodCommand(stateManager, "loadHigh", 0, false, 0));
    modes["TurnThreeTote"]->addConcurrent(new DriveCommand(stateManager, threeTote - closedOffset + driveBackOffset + 1.0, "hellaFast"));
    modes["TurnThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, false, 0));
    //modes["TurnThreeTote"]->addSequential(new SauropodCommand(stateManager, "loadLow", 0.5, false, 2));
    //modes["TurnThreeTote"]->addConcurrent(new SauropodCommand(stateManager, "loadHigh", 0, true, 1));
    modes["TurnThreeTote"]->addSequential(new TurnCommand(stateManager, 130, 0.9, 5));
    modes["TurnThreeTote"]->addSequential(new DriveCommand(stateManager, threeTote + 9.0, "hellaFast"));
    //modes["TurnThreeTote"]->addSequential(new TurnCommand(stateManager, 180, 0.9, 5));
    modes["TurnThreeTote"]->addSequential(new SauropodCommand(stateManager, "autoScore", 0.0, false, 0, true));
    modes["TurnThreeTote"]->addConcurrent(new IntakeCommand(stateManager, 1.0, true, 0));
    modes["TurnThreeTote"]->addConcurrent(new DriveCommand(stateManager, threeTote, "hellaFast"));
    modes["TurnThreeTote"]->addSequential(new IntakeCommand(stateManager, 0.0, true, 0));

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

void AutoManager::OnValueChange(std::string varName, std::string newValue)
{
    if (modes.find(newValue) != modes.end()) {
        setMode(newValue);
    }
    else
        printf("'%s' is not a valid auto mode - doing nothing\n", newValue.c_str());
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
