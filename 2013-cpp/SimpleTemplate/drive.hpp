#include "WPILib.h"

class Drive
{
protected:
private:
    Talon *leftDrive;
    Talon *rightDrive;
    Solenoid *lowGear;
    Solenoid *kickUp;
    bool highGear;
    float oldWheel;
    float negInertiaAccumulator;
    float quickStopAccumulator;
    void setDriveMotors(float left, float right);
    float limit(float x);
    void CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn);
public:
    Drive();
    void update(double DriveX, double DriveY, bool Gear, bool quickTurn);
    void setKickUp(bool k);
    void setBackWheelsDown(bool d);
    void setHighGear(bool g);
};
