#ifndef SAUROPOD_PATH_COMMAND_HPP
#define SAUROPOD_PATH_COMMAND_HPP

#include "autoCommand.hpp"

namespace frc973 {

class StateManager;

class SauropodPathCommand : public AutoCommand {
public:
    SauropodPathCommand(StateManager *manager_, int path_, float toteTimeout_, float timeout_);
    void init();
    bool taskPeriodic();
private:
    int path;
    float toteTimeout;
    bool moving;
    StateManager *manager;
};

}

#endif
