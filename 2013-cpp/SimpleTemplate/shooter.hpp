#include "WPILib.h"

class Shooter
{
protected:
    Talon *roller;
    Talon *flywheelMotor;
    Solenoid *indexer;
    Counter *flywheelCounter;
    Solenoid *shotAngle;
public:
    Shooter();
    float computeFlywheelSpeed(float counter);
    float getFlywheelSpeed();
    float RPMcontrol(float rpm);
    void setIndexer(bool t);
};
