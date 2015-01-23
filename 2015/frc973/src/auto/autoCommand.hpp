#ifndef AUTO_COMMAND_HPP
#define AUTO_COMMAND_HPP

#include "WPILib.h"

namespace frc973 {

class AutoCommand {
public:
    AutoCommand();
    void setTimeout(float timeout_);
    virtual void init();
    virtual bool taskPeriodic();
    bool run();

    virtual ~AutoCommand() {}

protected:
    Timer *timer;
    float timeout;

private:
    bool initYet;
    bool doneYet;
};

}

#endif
