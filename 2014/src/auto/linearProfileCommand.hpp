#include "autoCommand.hpp"

#ifndef DRIVE_PROFILE_COMMAND_H
#define DRIVE_PROFILE_COMMAND_H

class LinearProfileCommand : public AutoCommand
{
public:
    LinearProfileCommand(Drive *drive_, float target_, float aMax, float vMax, float dMax, float timeout_);
    virtual void Init();
    virtual bool Run();
private:
    Drive *drive;
    TrapProfile *profileGenerator;
    float target;
};

#endif
