#include "WPILib.h"

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
    bool highGear;
    float oldWheel;
    float negInertiaAccumulator;
    float quickStopAccumulator;
    void setDriveMotors(float left, float right);
    float limit(float x);
    void CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn);
    bool isLowGear;
    bool isKickUp;
public:
    Drive();
    void update(double DriveX, double DriveY, bool Gear, bool quickTurn);
    void setKickUp(bool k);
    void setBackWheelsDown(bool d);
    void setHighGear(bool g);
};
