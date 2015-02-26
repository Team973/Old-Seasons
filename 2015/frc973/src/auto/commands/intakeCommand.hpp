#ifndef INTAKE_COMMAND_HPP
#define INTAKE_COMMAND_HPP

#include "autoCommand.hpp"

namespace frc973 {

class StateManager;

class IntakeCommand : public AutoCommand {
public:
    IntakeCommand(StateManager *manager_, float speed_, bool open_, bool lastTote_, float timeout_);
    void init();
    bool taskPeriodic();
private:
    StateManager *manager;
    float speed;
    bool open;
    bool lastTote;
};

}

#endif
