#ifndef WAIT_COMMAND_HPP
#define WAIT_COMMAND_HPP

#include "autoCommand.hpp"

namespace frc973 {

class WaitCommand : public AutoCommand {
public:
    WaitCommand(float timeout_);
protected:
    void init();
    bool taskPeriodic();
};

}
#endif
