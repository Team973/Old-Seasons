#ifndef WAIT_HPP
#define WAIT_HPP

#include "autoCommand.hpp"

class Wait : public AutoCommand {
public:
    Wait(float timeout_);
    void init();
    bool run();
};

#endif
