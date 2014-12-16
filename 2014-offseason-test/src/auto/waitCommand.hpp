#ifndef WAIT_HPP
#define WAIT_HPP

#include "autoCommand.hpp"

namespace frc973 {

class WaitCommand : public AutoCommand {
public:
    WaitCommand(float timeout_);
    void init();
    bool run();
};

}

#endif
