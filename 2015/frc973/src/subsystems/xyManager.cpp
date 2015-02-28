#include "xyManager.hpp"
#include "WPILib.h"
#include "../lib/trapProfile.hpp"
#include "../lib/pid.hpp"
#include "../lib/utility.hpp"
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

    done = false;

    updateValue = new XYManager::MotorValue;

    locator = NULL;

    linearProfile = new TrapProfile(0, 0, 0, 0);

    loopTimer = new Timer();

    drivePID = new PID(
            Constants::getConstant("kDriveP")->getDouble(),
            Constants::getConstant("kDriveI")->getDouble(),
            Constants::getConstant("kDriveD")->getDouble());
    drivePID->start();
    drivePID->setBounds(-.6,.6);
    turnPID = new PID(
            Constants::getConstant("kTurnP")->getDouble(),
            Constants::getConstant("kTurnI")->getDouble(),
            Constants::getConstant("kTurnD")->getDouble());
    turnPID->start();
    turnPID->setBounds(-.7,.7);

    relativeDistance = 0;

    isPaused = true;

    speedLimit = 0.6;

    printf("Profile Pos, Profile Vel, Profile Accel, Actual Pos, Actual Vel\n");
}

void XYManager::setSpeed(bool fast) {
    if (fast) {
        speedLimit = 0.5;
    } else {
        speedLimit = 0.3;
    }
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
    linearProfile = new TrapProfile(distance_ - currPoint.distance, 10000, 10000, 10000);
    turnPID->setTarget(currPoint.angle);
    drivePID->setBounds(-.6,.6);
    done = false;
}

void XYManager::setTargetAngle(float angle_)
{
    currPoint = locator->getPoint();
    turnPID->setTarget(angle_);
    drivePID->setBounds(0.0,0.0);
    done = false;
}

void XYManager::resetProfile() {
    loopTimer->Reset();
}

void XYManager::startProfile()
{
    loopTimer->Start();
    isPaused = false;
}

void XYManager::pauseProfile() {
    loopTimer->Stop();
    isPaused = true;
}

XYManager::MotorValue* XYManager::getValues()
{
    return updateValue;
}

float XYManager::getDistanceFromTarget() {
    return fabs(linearProfile->getTarget() - relativeDistance);
}

void XYManager::update()
{
    locator->update();
    
    currPoint = locator->getPoint();

    float currTime = loopTimer->Get();

    std::vector<float> linearStep = linearProfile->getProfile(currTime);

    float linearFF = (kLinVelFF*linearStep[2]) + (kLinAccelFF*linearStep[3]);

    relativeDistance = currPoint.distance - origPoint.distance;

    float angleError = turnPID->getTarget() - currPoint.angle;

    if (linearProfile->getTarget() - relativeDistance <= .05 && angleError <= 3 && locator->getLinearVelocity() < 2) {
        done = true;
    }

    float driveInput = 0;
    float angularInput = 0;

    SmartDashboard::PutString("DB/String 4", asString(currPoint.angle));
    SmartDashboard::PutString("DB/String 5", asString(relativeDistance));
    SmartDashboard::PutString("DB/String 6", asString(linearProfile->getTarget()));
    SmartDashboard::PutString("DB/String 7", asString(currPoint.distance));
    SmartDashboard::PutString("DB/String 8", asString(origPoint.distance));

    printf("%f, %f, %f, %f, %f, %f\n",linearStep[0], linearStep[1], linearStep[2], linearStep[3], relativeDistance, locator->getLinearVelocity());

    driveInput = drivePID->update(relativeDistance - linearStep[1], loopTimer) + linearFF;
    angularInput = -(turnPID->update(currPoint.angle, loopTimer));

    if (!isPaused && (linearProfile->getTarget() - relativeDistance) > 0) {
        driveInput += 0.1;
    }

    if (driveInput > speedLimit) {
        driveInput = speedLimit;
    } else if (driveInput < -speedLimit) {
        driveInput = -speedLimit;
    }

    updateValue->throttle = driveInput;
    updateValue->turn = angularInput;
}
}
