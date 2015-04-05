#ifndef INTAKE_COMMAND_HPP
#define INTAKE_COMMAND_HPP

#include "autoCommand.hpp"

namespace frc973 {

class StateManager;

class IntakeCommand : public AutoCommand {
public:
    IntakeCommand(StateManager *manager_, float speed_, bool position_, float timeout_);
    IntakeCommand(StateManager *manager_, float leftSpeed_, float rightSpeed_, bool position_, float timeout_);
    void init();
    bool taskPeriodic();
private:
    StateManager *manager;
    float leftSpeed;
    float rightSpeed;
    bool position;
};

}

#endif
