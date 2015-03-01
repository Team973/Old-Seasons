#ifndef GRAB_COMMAND_HPP
#define GRAB_COMMAND_HPP

#include "autoCommand.hpp"

namespace frc973 {

class StateManager;

class GrabCommand : public AutoCommand {
public:
    GrabCommand(StateManager *manager_, bool grab_);
    void init();
    bool taskPeriodic();
private:
    StateManager *manager;
    bool grab;
};

}

#endif
