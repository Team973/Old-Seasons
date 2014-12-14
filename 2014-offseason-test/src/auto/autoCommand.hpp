#ifndef AUTO_COMMAND_HPP
#define AUTO_COMMAND_HPP

#include "WPILib.h"

class AutoCommand {
public:
    AutoCommand();
    virtual void init();
    virtual bool run();
    void setTimeout(float timeout_);

    virtual ~AutoCommand() {}

    Timer *timer;
    float timeout;
};

#endif
