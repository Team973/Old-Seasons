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

    float normalizeAngle(float theta);

    // Talons
    Talon *leftDrive;
    Talon *rightDrive;

    // Encoders
    Encoder *leftEncoder;
    Encoder *rightEncoder;

    Solenoid *shifters;
    Solenoid *kickUp;

public:
    Drive(Talon *leftDrive_, Talon *rightDrive_, Solenoid *shifters_, Solenoid *kickUp_, Encoder *leftEncoder_, Encoder *rightEncoder_);
    void update(double DriveX, double DriveY, bool gear, bool kick, bool quickTurn);
    void dashboardUpdate();

    float getLeftDrive();
    float getRightDrive();
    float getLeftDistance();
    float getRightDistance();
    float getWheelDistance();
    void resetDriveEncoders();
    void resetGyro();
    float getGyroAngle();
    void resetDrive();
    void setLowGear(bool lowGear);
    void setKickUp(bool kick);
};

#endif
