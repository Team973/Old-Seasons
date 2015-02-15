#include "WPILib.h"

#ifndef AUTOCOMMAND_H
#define AUTOCOMMAND_H

class AutoCommand
{
public:
    AutoCommand();
    virtual void Init();
    virtual bool Run();
    void setTimeout(double timeout_);
    void injectTimer(Timer *autoTimer_);

    double timeout;
    Timer* timer; // for internal timeouts
    Timer* autoTimer; // for overall auto timing

    virtual ~AutoCommand() {};
};

#endif
