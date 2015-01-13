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
    void setTargetDistance(float distance_);
    void setTargetAngle(float angle_);
    void startProfile();
    XYManager::MotorValue* getValues();
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
};

}

#endif
