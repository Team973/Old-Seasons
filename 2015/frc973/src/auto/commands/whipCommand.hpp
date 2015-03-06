#ifndef WHIP_COMMAND_HPP
#define WHIP_COMMAND_HPP

#include "autoCommand.hpp"

namespace frc973 {

class StateManager;

class WhipCommand : public AutoCommand {
public:
    WhipCommand(StateManager *manager_, std::string position_, float timeout_);
    void init();
    bool taskPeriodic();
private:
    StateManager *manager;
    std::string position;
};

}

#endif
