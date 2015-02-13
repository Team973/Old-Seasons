#include "WPILib.h"
#include "autoCommand.hpp"

class AutoWaitCommand : public AutoCommand
{
public:
    AutoWaitCommand(float timeout_);
    virtual void Init();
    virtual bool Run();
private:
};
