#include "autoCommand.hpp"
#include "../pid.hpp"

#ifndef LINEAR_DRIVE_COMMAND_H
#define LINEAR_DRIVE_COMMAND_H

class Drive;

class LinearDriveCommand : public AutoCommand
{
public:
    LinearDriveCommand(Drive *drive_, float targetDrive_, float targetAngle_=0, bool backwards_=false, float timeout_=0, float drivePercision_=6);
    virtual void Init();
    virtual bool Run();
private:
    Drive *drive;
    float targetDrive;
    float targetAngle;
    bool backwards;
    float drivePrecision;

    PID *drivePID;
    PID *anglePID;
};

#endif
