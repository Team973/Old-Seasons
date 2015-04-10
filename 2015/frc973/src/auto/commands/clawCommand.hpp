#ifndef CLAW_COMMAND_HPP
#define CLAW_COMMAND_HPP

#include "autoCommand.hpp"

namespace frc973 {

class StateManager;

class ClawCommand : public AutoCommand {
public:
    ClawCommand(StateManager *manager_, bool open_);
    void init();
    bool taskPeriodic();
private:
    StateManager* manager;
    bool open;
};

}

#endif
