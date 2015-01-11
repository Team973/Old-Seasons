#ifndef X_Y_MANAGER_HPP
#define X_Y_MANAGER_HPP

#include "locator.hpp"

namespace frc973 {

class Locator;
class PID;
class TrapProfile;

class XYManager {
public:

    struct MotorValue {
        float throttle = 0;
        float turn = 0;
    };

    XYManager();
    static XYManager* getInstance();
    void injectLocator(Locator* locator_);
    void reset();
    void setTarget(float targetX_, float targetY, bool backward_, float drivePrecision_, float turnPrecision_, float driveCap_, float turnCap_, float arcCap_);
    void startProfile();
    XYManager::MotorValue* getValues();
    void calculate();
    void update();

private:
    static XYManager* instance;

    Locator* locator;
    Locator::Point* currPoint;
    Locator::Point* origPoint;

    XYManager::MotorValue* updateValue;

    Timer* loopTimer;

    TrapProfile* linearProfile;
    TrapProfile* angularProfile;

    PID* drivePID;
    PID* turnPID;

    float kLinVelFF;
    float  kLinAccelFF;
    float kAngVelFF;
    float kAngAccelFF;

    float targetX;
    float targetY;
    float drivePrecision;
    float turnPrecision;
    float driveCap;
    float turnCap;
    float arcCap;
    bool backward;

    float targetAngle;
    float driveError;
    float angleError;
    float robotLinearError;
};

}

#endif
