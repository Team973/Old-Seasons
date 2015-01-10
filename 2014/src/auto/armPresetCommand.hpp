#include "autoCommand.hpp"
#include "../arm.hpp"

#ifndef ARM_PRESET_COMMAND_H
#define ARM_PRESET_COMMAND_H

class ArmPresetCommand : public AutoCommand
{
public:
    ArmPresetCommand(Arm *arm_, int preset_, float timeout_);
    virtual void Init();
    virtual bool Run();
private:
    int preset;
    Arm *arm;
};

#endif
