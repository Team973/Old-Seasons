#ifndef DRIVE_H
#define DRIVE_H

class Drive
{
public:
    Drive(Talon *left_, Talon *right_, Solenoid *shifters_);
    void setDriveMotors(float left, float right);
    void CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn);
    void update();
private:

    float limit(float x);

    Talon *leftMotor;
    Talon *rightMotor;
    Solenoid *shifters;

    float leftPower;
    float rightPower;

    float quickStopAccumulator;
    float negInertiaAccumulator;
    float oldWheel;
};

#endif
