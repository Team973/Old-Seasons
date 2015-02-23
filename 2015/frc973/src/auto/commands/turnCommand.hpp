#ifndef TURN_COMMAND_HPP
#define TURN_COMMAND_HPP

#include "autoCommand.hpp"

namespace frc973 {

class XYManager;
class StateManager;

class TurnCommand : public AutoCommand {
public:
    TurnCommand(StateManager *manager_, float target_, float timeout_);
    void init();
    bool taskPeriodic();
private:
    XYManager *xyManager;

    StateManager *manager;

    float target;
};

}
#endif
