#include "xyManager.hpp"
#include "WPILib.h"
#include "../lib/trapProfile.hpp"
#include "../lib/pid.hpp"
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
    reset();
    targetX = 0;
    targetY = 0;
    drivePrecision = 0;
    turnPrecision = 0;
    driveCap = 0;
    turnCap = 0;
    arcCap = 0;

    kLinVelFF = 0.08;
    kLinAccelFF = 0.03;
    kAngVelFF = 0.0;
    kAngAccelFF = 0.0;

    currPoint = new Locator::Point;
    origPoint = new Locator::Point;

    updateValue = new XYManager::MotorValue;

    linearProfile = new TrapProfile();
    angularProfile = new TrapProfile();

    loopTimer = new Timer();

    drivePID = new PID(0, 0, 0);
    drivePID->start();
    drivePID->setBounds(0,0);
    turnPID = new PID(0, 0, 0);
    turnPID->start();
    turnPID->setBounds(0,0);
}

void XYManager::injectLocator(Locator* locator_)
{
    locator = locator_;
}

void XYManager::reset()
{
    targetAngle = 0;
    driveError = 0;
    angleError = 0;
    robotLinearError = 0;
}

void XYManager::setTarget(float targetX_, float targetY_, bool backward_, float drivePrecision_, float turnPrecision_, float driveCap_, float turnCap_, float arcCap_)
{
    targetX = targetX_;
    targetY = targetY_;
    backward = backward_;
    drivePrecision = drivePrecision_;
    turnPrecision = turnPrecision_;
    driveCap = driveCap_;
    turnCap = turnCap_;
    arcCap = arcCap_;
    reset();
    currPoint = locator->getPoint();
    origPoint = locator->getPoint();
    calculate();

    drivePID->setBounds(-driveCap, driveCap);
    turnPID->setBounds(-turnCap, turnCap);

    linearProfile = new TrapProfile(driveError, 8, 10, 15);
    angularProfile = new TrapProfile(angleError, 100000, 100000,10000); // this is purposfully blown up do not change the numbers
}

void XYManager::startProfile()
{
    loopTimer->Start();
    loopTimer->Reset();
}

void XYManager::calculate()
{
    targetAngle = atan2(currPoint->x - targetX, targetY - currPoint->y)/M_PI*180;
    driveError = sqrt(pow(targetX - currPoint->x, 2) + pow(targetY - currPoint->y, 2));
    angleError = targetAngle - currPoint->angle;

    if (backward)
    {
        targetAngle += 180;
    }

    while (targetAngle > 180)
    {
        targetAngle -= 360;
    }

    robotLinearError = (targetY - currPoint->y) * cos(currPoint->angle/180*M_PI) - (targetX - currPoint->x) * sin(currPoint->angle/180*M_PI);
}

XYManager::MotorValue* XYManager::getValues()
{
    return updateValue;
}

void XYManager::update()
{
    currPoint = locator->getPoint();
    calculate();

    float currTime = loopTimer->Get();

    std::vector<float> linearStep = linearProfile->getProfile(currTime);
    std::vector<float> angularStep = angularProfile->getProfile(currTime);

    float linearFF = -(kLinVelFF*linearStep[2]) + -(kLinAccelFF*linearStep[3]);
    float angularFF = -(kAngVelFF*angularStep[2]) + -(kAngAccelFF*angularStep[3]);

    float distanceError = currPoint->distance - origPoint->distance;

    float driveInput, angularInput;

    if (fabs(angleError) < turnPrecision)
    {
        if (fabs(robotLinearError) < drivePrecision)
        {
            driveInput = 0;
            angularInput = 0;
        }
        else
        {
            driveInput = drivePID->update(linearStep[1] - distanceError, loopTimer) + linearFF;
            angularInput = turnPID->update(angularStep[1] - currPoint->angle, loopTimer) + angularFF;
        }
    }
    else
    {
        driveInput = 0;
        angularInput = turnPID->update(angularStep[1] - currPoint->angle, loopTimer) + angularFF;
    }

    if (backward)
    {
        driveInput *= -1;
    }

    updateValue->throttle = driveInput;
    updateValue->turn = angularInput;
}

}
