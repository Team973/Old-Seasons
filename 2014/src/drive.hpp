#include "WPILib.h"

#ifndef DRIVE_H
#define DRIVE_H

class Drive
{
private:
    float limit(float x);
    void setDriveMotors(float left, float right);
    void CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn);
    float signSquare(float x);
    void arcade(float move_, float rotate_);

    float oldWheel;
    float negInertiaAccumulator;
    float quickStopAccumulator;
    float leftDist;
    float rightDist;
    double M_PI;

    // Talons
    Victor *leftDrive;
    Victor *rightDrive;

    // Encoders
    Encoder *leftEncoder;
    Encoder *rightEncoder;

    // Gyro
    Gyro *gyro;
public:
    Drive(Victor *leftDrive_, Victor *rightDrive_, Encoder *leftEncoder_, Encoder *rightEncoder_, Gyro *gyro_);
    void update(double DriveX, double DriveY, bool gear, bool quickTurn);
    void dashboardUpdate();

    float getLeftDrive();
    float getRightDrive();
    float getLeftDistance();
    float getRightDistance();
    float getWheelDistance();
    void resetDriveEncoders();
    void resetGyro();
};

#endif
