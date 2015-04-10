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
#include "lib/socketClient.hpp"

namespace frc973 {

AutoManager::AutoManager(StateManager *stateManager_)
{
    stateManager = stateManager_;
    resetModes();
    SocketClient::AddListener("autoMode", this);
}

void AutoManager::setModes() {
    modes["ThreeTote"] = new AutoSequencer();
    modes["Drive"] = new AutoSequencer();
    modes["Turn"] = new AutoSequencer();
    modes["Grab"] = new AutoSequencer();
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

    modes["Turn"]->addSequential(new TurnCommand(stateManager, 90, 15));

    modes["Grab"]->addSequential(new GrabCommand(stateManager, true));
    modes["Grab"]->addSequential(new WaitCommand(2));
    modes["Grab"]->addSequential(new DriveCommand(stateManager, 9, "hellaFast"));
    modes["Grab"]->addSequential(new GrabCommand(stateManager, false));

    modes["None"]->addSequential(new WaitCommand(10000));

    modes["ThreeTote"]->addConcurrent(new SauropodCommand(stateManager, "loadHigh", 0, false, 0));
    modes["ThreeTote"]->addConcurrent(new IntakeCommand(stateManager, 1.0, "float", 0));
    modes["ThreeTote"]->addConcurrent(new DriveCommand(stateManager, twoTote - openOffset, "slow"));
    modes["ThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, "open", 0));
    modes["ThreeTote"]->addSequential(new DriveCommand(stateManager, twoTote - closedOffset, "fast"));
    modes["ThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, "float", 0));
    modes["ThreeTote"]->addSequential(new SauropodCommand(stateManager, "loadLow", 0.5, false, 2));
    modes["ThreeTote"]->addConcurrent(new SauropodCommand(stateManager, "loadHigh", 0, false, 0));
    modes["ThreeTote"]->addConcurrent(new IntakeCommand(stateManager, 1.0, "float", 0));
    modes["ThreeTote"]->addConcurrent(new DriveCommand(stateManager, threeTote - openOffset, "slow"));
    modes["ThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, "open", 0));
    modes["ThreeTote"]->addSequential(new DriveCommand(stateManager, threeTote - closedOffset, "fast"));
    modes["ThreeTote"]->addSequential(new IntakeCommand(stateManager, -1.0, "float", 0));
    modes["ThreeTote"]->addSequential(new SauropodCommand(stateManager, "loadLow", 0.5, false, 2));
    modes["ThreeTote"]->addConcurrent(new IntakeCommand(stateManager, 0.0, "closed", 0));
    modes["ThreeTote"]->addConcurrent(new SauropodCommand(stateManager, "rest", 0, true, 1));
    modes["ThreeTote"]->addSequential(new TurnCommand(stateManager, 90, 5));
    modes["ThreeTote"]->addConcurrent(new SauropodCommand(stateManager, "autoScore", 2.75, false, 0, true));
    modes["ThreeTote"]->addConcurrent(new IntakeCommand(stateManager, 0.0, "open", 0));
    modes["ThreeTote"]->addConcurrent(new DriveCommand(stateManager, 0, "hellaFast"));

    modes["Test"]->addConcurrent(new SauropodCommand(stateManager, "rest", 0, false, 0));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, 1.0, "float", 2));
    modes["Test"]->addConcurrent(new SauropodCommand(stateManager, "autoScore", 1.75, false, 0, true));
    modes["Test"]->addConcurrent(new IntakeCommand(stateManager, 0.0, "open", 0));
    modes["Test"]->addConcurrent(new DriveCommand(stateManager, -7.5, "hellaFast"));

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
