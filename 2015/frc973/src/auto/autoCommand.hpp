#ifndef AUTO_COMMAND_HPP
#define AUTO_COMMAND_HPP

#include "WPILib.h"

namespace frc973 {

class AutoCommand {
public:
    AutoCommand();
    bool run();
    void setTimeout(float timeout_);

    virtual ~AutoCommand() {}

protected:
    virtual void init();
    virtual bool taskPeriodic();

    Timer *timer;
    float timeout;

private:
    bool initYet;
    bool doneYet;
};

}

#endif
