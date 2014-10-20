#ifndef DRIVE_H
#define DRIVE_H

#include "lib/subsystemBase.hpp"

class Drive : public SubsystemBase
{
public:
    Drive(Talon *frontLeft_, Talon *frontRight_, Talon *backLeft_, Talon *backRight_, Talon *strafe_, Solenoid *shifters_);
    void setBehavior(float throttle, float turn, bool highGear, bool quickTurn);
    void strafe(float z);
    void update();
private:

    void setDriveMotors(float left, float right);
    void CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn);

    Talon *frontLeftMotor;
    Talon *frontRightMotor;
    Talon *backLeftMotor;
    Talon *backRightMotor;
    Talon *strafeMotor;
    Solenoid *shifters;

    float leftPower;
    float rightPower;

    float quickStopAccumulator;
    float negInertiaAccumulator;
    float oldWheel;
};

#endif
