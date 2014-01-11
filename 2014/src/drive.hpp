#include "WPILib.h"

#ifndef DRIVE_H
#define DRIVE_H

class Drive
{
private:
    float limit(float x);
    void setDriveMotors(float left, float right);
    void CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn);

    // Talons
    Talon *leftDrive;
    Talon *rightDrive;

    // Encoders
    Encoder *leftEncoder;
    Encoder *rightEncoder;
public:
    Drive(Talon *leftDrive_, Talon *rightDrive_, Encoder *leftEncoder_, Encoder *rightEncoder_);
    void update(double DriveX, double DriveY, bool gear, bool quickTurn);
};

#endif
