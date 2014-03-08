#include "WPILib.h"

#ifndef DRIVE_H
#define DRIVE_H

class Drive
{
private:
    float limit(float x);
    void setDriveMotors(float left, float right);
    float signSquare(float x);
    void CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn);
    void arcade(float move_, float rotate_);

    float quickStopAccumulator;
    float negInertiaAccumulator;
    float oldWheel;
    float leftDist;
    float rightDist;
    double M_PI; // Apparently not included in math.h

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
    void update(double DriveX, double DriveY, bool gear, bool kick, bool quickTurn, bool isAuto=false);
    void dashboardUpdate();

    float getLeftDrive();
    float getRightDrive();
    float getLeftDistance();
    float getRightDistance();
    float getWheelDistance();
    void resetDriveEncoders();
    //TODO(oliver): Add in the reset gyro function
    float getGyroAngle();
    void resetDrive();
    void setLowGear(bool lowGear);
    void setKickUp(bool kick);
};

#endif
