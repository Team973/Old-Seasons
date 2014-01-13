#include "WPILib.h"

#ifndef DRIVE_H
#define DRIVE_H

class Drive
{
private:
    float limit(float x);
    void setDriveMotors(float left, float right);
    void CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn);

    float oldWheel;
    float negInertiaAccumulator;
    float quickStopAccumulator;

    // Talons
    Victor *leftDrive;
    Victor *rightDrive;

    // Encoders
    Encoder *leftEncoder;
    Encoder *rightEncoder;
public:
    Drive(Victor *leftDrive_, Victor *rightDrive_, Encoder *leftEncoder_, Encoder *rightEncoder_);
    void update(double DriveX, double DriveY, bool gear, bool quickTurn);
};

#endif
