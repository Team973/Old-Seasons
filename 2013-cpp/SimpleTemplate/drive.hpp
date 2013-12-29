#include "WPILib.h"

#ifndef DRIVE_H
#define DRIVE_H

class Drive
{
protected:
private:
    Talon *frontLeftDrive;
    Talon *frontRightDrive;
    Talon *backLeftDrive;
    Talon *backRightDrive;

    Solenoid *lowGear;
    Solenoid *kickUp;
    float oldWheel;
    float negInertiaAccumulator;
    float quickStopAccumulator;
    float limit(float x);
    void CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn);
    void mecanumDrive(double x, double y, double z);
    bool isLowGear;
    bool isKickUp;
    void setFrontLeftDrive(float speed);
    void setBackLeftDrive(float speed);
    void setFrontRightDrive(float speed);
    void setBackRightDrive(float speed);

public:
    Drive();
    void setDriveMotors(float left, float right);
    void setLeftDrive(float speed);
    void setRightDrive(float speed);
    void update(double DriveX, double DriveY, double DriveZ, bool Gear, bool quickTurn);
    void setKickUp(bool k);
    void setBackWheelsDown(bool d);
    void setHighGear(bool g);

};

#endif
