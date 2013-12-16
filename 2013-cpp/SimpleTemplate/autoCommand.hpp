#include "WPILib.h"

class AutoCommand
{
public:
    AutoCommand();
    virtual void Init();
    virtual bool Run();
    void setTimeout(double timeout_);

    double timeout;
    Timer* timer;

    virtual ~AutoCommand();
};
