#ifndef X_Y_MANAGER_HPP
#define X_Y_MANAGER_HPP

#include "locator.hpp"

namespace frc973 {

class Locator;

class XYManager {
public:

    struct MotorValue {
        float throttle;
        float turn;
    };

    XYManager();
    XYManager* getInstance();
    void injectLocator(Locator* locator_);
    void reset();
    void setTarget(float targetX_, float targetY, bool backward_, float drivePrecision_, float turnPrecision_, float driveCap_, float turnCap_, float arcCap_);
    void calculate();
    void update();

private:
    static XYManager* instance;

    Locator* locator;
    Locator::Point* currPoint;

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
