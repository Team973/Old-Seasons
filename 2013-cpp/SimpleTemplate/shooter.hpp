#include "WPILib.h"

class Shooter
{
protected:
    Talon *roller;
    Talon *flywheelMotor;
    Solenoid *indexer;
    Counter *flywheelCounter;
    Solenoid *shotAngle;
    bool rollerRunning;
    bool flywheelRunning;
    bool firingIndexer;
    int discsFired;
    int targetFlywheelSpeed;
public:
    Shooter();
    float computeFlywheelSpeed(float counter);
    float getFlywheelSpeed();
    float RPMcontrol(float rpm);
    void setIndexer(bool t);
    void setRollerRunning(bool r);
    void setFlywheelRunning(bool f);
    void setShotAngle(bool h);
    void clearDiscsFired();
    void autoFire();
    void update();
    void dashboardUpdate();
};
