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
    void setSpeed(bool fast);
    bool isMovementDone();
    void setTargetDistance(float distance_);
    void setTargetAngle(float angle_);
    void startProfile();
    void pauseProfile();
    void resetProfile();
    void resetEncoders();
    XYManager::MotorValue* getValues();
    float getDistanceFromTarget();
    void update();

private:
    static XYManager* instance;

    Locator* locator;
    Locator::Point currPoint;
    Locator::Point origPoint;

    XYManager::MotorValue* updateValue;

    Timer* loopTimer;

    TrapProfile* linearProfile;

    PID* drivePID;
    PID* turnPID;

    float kLinVelFF;
    float  kLinAccelFF;
    float kAngVelFF;
    float kAngAccelFF;

    float relativeDistance;

    float speedLimit;

    bool isPaused;

    bool done;
};

}

#endif
