#include "xyManager.hpp"
#include "WPILib.h"
#include "../lib/trapProfile.hpp"
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
    currPoint = new Locator::Point;
    currPoint->x = 0;
    currPoint->y = 0;

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
    calculate();
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

void XYManager::update()
{
    currPoint = locator->getPoint();
}

}
