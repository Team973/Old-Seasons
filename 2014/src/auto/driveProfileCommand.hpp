#include "autoCommand.hpp"

#ifndef DRIVE_PROFILE_COMMAND_H
#define DRIVE_PROFILE_COMMAND_H

class Drive;

class DriveProfileCommand : public AutoCommand
{
public:
    DriveProfileCommand(Drive *drive_, float linearTarget_, float angularTarget_, float timeout_);
    virtual void Init();
    virtual bool Run();
private:
    Drive *drive;
    float linearTarget;
    float angularTarget;
};

#endif
