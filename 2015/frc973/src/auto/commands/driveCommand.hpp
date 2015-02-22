#ifndef DRIVE_COMMAND_HPP
#define DRIVE_COMMAND_HPP

#include "autoCommand.hpp"

namespace frc973 {

class XYManager;
class FlagAccumulator;

class DriveCommand : public AutoCommand {
public:
    DriveCommand(float target_, float timeout_);
    DriveCommand(AutoCommand *cmdSeq_, float distance_, float target_, float timeout_);
    void init();
    bool taskPeriodic();
private:
    AutoCommand *cmdSeq;
    float distance;
    float target;
    XYManager *xyManager;
    FlagAccumulator *accumulator;
    bool done;
};

}
#endif
