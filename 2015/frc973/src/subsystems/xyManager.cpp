#include "xyManager.hpp"
#include "WPILib.h"
#include "../lib/trapProfile.hpp"
#include "../lib/pid.hpp"
#include "../constants.hpp"
#include <math.h>
#include <vector>

namespace frc973 {

XYManager* XYManager::instance = NULL;

XYManager* XYManager::getInstance()
{
    if (instance == NULL)
    {
        instance = new XYManager();
    }
    return instance;
}

XYManager::XYManager()
{
    kLinVelFF = Constants::getConstant("kLinVelFF")->getDouble();
    kLinAccelFF = Constants::getConstant("kLinAccelFF")->getDouble();
    kAngVelFF = Constants::getConstant("kAngVelFF")->getDouble();
    kAngAccelFF = Constants::getConstant("kAngAccelFF")->getDouble();

    done = true;

    currPoint = new Locator::Point;
    origPoint = new Locator::Point;

    updateValue = new XYManager::MotorValue;

    linearProfile = new TrapProfile();
    angularProfile = new TrapProfile();

    loopTimer = new Timer();

    drivePID = new PID(0.0, 0, 0);
    drivePID->start();
    drivePID->setBounds(-1,1);
    turnPID = new PID(0.0, 0, 0);
    turnPID->start();
    turnPID->setBounds(-1,1);
}

void XYManager::injectLocator(Locator* locator_)
{
    locator = locator_;
}

bool XYManager::isMovementDone()
{
    return done;
}

void XYManager::setTargetDistance(float distance_)
{
    currPoint = locator->getPoint();
    origPoint = locator->getPoint();
    linearProfile = new TrapProfile(distance_, 8, 10, 15);
    angularProfile = new TrapProfile(currPoint->angle, 100000, 100000,10000); // this is purposfully blown up do not change the numbers
    done = false;
}

void XYManager::setTargetAngle(float angle_)
{
    angularProfile = new TrapProfile(angle_, 100000, 100000,10000); // this is purposfully blown up do not change the numbers
    linearProfile = new TrapProfile(0, 8, 10, 15); // this means that we don't have to seperate turn and drive in update
    done = false;
}

void XYManager::startProfile()
{
    loopTimer->Start();
    loopTimer->Reset();
}

XYManager::MotorValue* XYManager::getValues()
{
    return updateValue;
}

void XYManager::update()
{
    currPoint = locator->getPoint();

    float currTime = loopTimer->Get();

    std::vector<float> linearStep = linearProfile->getProfile(currTime);
    std::vector<float> angularStep = angularProfile->getProfile(currTime);

    float linearFF = (kLinVelFF*linearStep[2]) + (kLinAccelFF*linearStep[3]);
    float angularFF = (kAngVelFF*angularStep[2]) + (kAngAccelFF*angularStep[3]);

    float distanceError = fabs(currPoint->distance - origPoint->distance);
    float angleError = fabs(currPoint->angle - origPoint->distance);

    if (distanceError <= .5 && angleError <= 2)
    {
        done = true;
    }
    else
    {
        done = false;
    }

    float driveInput, angularInput;

    driveInput = -drivePID->update(linearStep[1] - distanceError, loopTimer) + linearFF;
    angularInput = turnPID->update(angularStep[1] - currPoint->angle, loopTimer) + angularFF;

    updateValue->throttle = driveInput;
    updateValue->turn = angularInput;
}

}
