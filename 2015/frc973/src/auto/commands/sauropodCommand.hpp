#ifndef SAUROPOD_COMMAND_HPP
#define SAUROPOD_COMMAND_HPP

#include "autoCommand.hpp"

namespace frc973 {

class StateManager;

class SauropodCommand : public AutoCommand {
public:
    SauropodCommand(StateManager *manager_, std::string preset_, float toteTimeout_, bool braked_, float timeout_);
    void init();
    bool taskPeriodic();
private:
    std::string preset;
    float toteTimeout;
    bool moving;
    bool braked;
    StateManager *manager;
};

}

#endif
