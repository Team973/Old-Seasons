#include "WPILib.h"

#ifndef DRIVE_H
#define DRIVE_H

class Drive
{
private:
    void CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn);
public:
    Drive();
    void update(double DriveX, double DriveY, bool gear, bool quickTurn);
};

#endif
