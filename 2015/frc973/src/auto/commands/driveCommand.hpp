#ifndef DRIVE_COMMAND_HPP
#define DRIVE_COMMAND_HPP

#include "autoCommand.hpp"

namespace frc973 {

class XYManager;
class FlagAccumulator;
class StateManager;

class DriveCommand : public AutoCommand {
public:
    DriveCommand(StateManager *manager_, float target_, bool fast_, float timeout_);
    DriveCommand(StateManager *manager_, AutoCommand *cmdSeq_, float distance_, float target_, bool fast_, float timeout_);
    void init();
    bool taskPeriodic();
private:
    StateManager *manager;
    AutoCommand *cmdSeq;
    float distance;
    float target;
    XYManager *xyManager;
    FlagAccumulator *accumulator;
    bool done;
};

}
#endif
