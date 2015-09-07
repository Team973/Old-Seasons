#ifndef TIMER_DRIVE_COMMAND_HPP
#define TIMER_DRIVE_COMMAND_HPP

#include "autoCommand.hpp"

namespace frc973 {

class StateManager;

class TimerDriveCommand : public AutoCommand {
public:
    TimerDriveCommand(StateManager *manager_, int direction_, float time_);
    void init();
    bool taskPeriodic();
private:
    StateManager *manager;
    int direction;
};

}

#endif
