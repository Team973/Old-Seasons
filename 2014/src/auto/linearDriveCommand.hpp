#include "WPILib.h"
#include "autoCommand.hpp"
#include "../drive.hpp"
#include "../pid.hpp"

#ifndef LINEAR_DRIVE_COMMAND_H
#define LINEAR_DRIVE_COMMAND_H

class LinearDriveCommand : public AutoCommand
{
public:
    LinearDriveCommand(Drive *drive_, float targetDrive_, float targetAngle_, bool backwards_, float timeout_, float drivePercision_=6, float arcPercision_=5);
    virtual void Init();
    virtual bool Run();
private:
    Drive *drive;
    float targetDrive;
    float targetAngle;
    bool backwards;
    float drivePrecision;
    float arcPrecision;

    PID *drivePID;
    PID *anglePID;
};

#endif
