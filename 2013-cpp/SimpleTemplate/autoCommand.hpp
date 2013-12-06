#include "WPILib.h"

class AutoCommand
{
public:
    AutoCommand();
    bool Init();
    bool Run();
    void setTimeout(double timeout_);

    double timeout;
    Timer* timer;
};
