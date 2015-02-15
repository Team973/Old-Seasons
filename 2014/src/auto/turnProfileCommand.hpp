#include "autoCommand.hpp"

#ifndef TURN_PROFILE_COMMAND
#define TURN_PROFILE_COMMAND

class Drive;
class TrapProfile;

class TurnProfileCommand : public AutoCommand
{
public:
    TurnProfileCommand(Drive *drive_, float target_, float aMax, float vMax, float dMax, float timeout_);
    virtual void Init();
    virtual bool Run();
private:
    Drive *drive;
    float target;
    TrapProfile *profileGenerator;
};

#endif
