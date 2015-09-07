#ifndef GRAB_COMMAND_HPP
#define GRAB_COMMAND_HPP

#include "autoCommand.hpp"

namespace frc973 {

class GrabManager;

class GrabCommand : public AutoCommand {
public:
    GrabCommand(GrabManager* grabber_, bool retract_=false);
    void init();
    bool taskPeriodic();
private:
    GrabManager *grabber;
    bool retract;
};

}

#endif
