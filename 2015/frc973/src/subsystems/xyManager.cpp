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
    angularProfile = new TrapProfile(0, 0, 0, 0);

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
    turnPID->setBounds(-.5,.5);

    relativeDistance = 0;

    speedLimit = 0.6;

    printf("Profile Pos, Profile Vel, Profile Accel, Actual Pos, Actual Vel\n");
}

void XYManager::setSpeed(bool fast) {
    if (fast) {
        speedLimit = 0.8;
    } else {
        speedLimit = 0.6;
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
    linearProfile = new TrapProfile(distance_ - currPoint.distance, 8.0, 10.0, 15.0);
    angularProfile = new TrapProfile(currPoint.angle, 100000, 100000,10000); // this is purposfully blown up do not change the numbers
    done = false;
}

void XYManager::setTargetAngle(float angle_)
{
    currPoint = locator->getPoint();
    angularProfile = new TrapProfile(angle_, 100000, 100000,10000); // this is purposfully blown up do not change the numbers
    linearProfile = new TrapProfile(currPoint.distance - origPoint.distance, 0, 0, 0); // this means that we don't have to seperate turn and drive in update
    done = false;
}

void XYManager::resetProfile() {
    loopTimer->Reset();
}

void XYManager::startProfile()
{
    loopTimer->Start();
}

void XYManager::pauseProfile() {
    loopTimer->Stop();
}

XYManager::MotorValue* XYManager::getValues()
{
    return updateValue;
}

float XYManager::getDistanceFromTarget() {
    return fabs(relativeDistance - linearProfile->getTarget());
}

void XYManager::update()
{
    locator->update();
    
    currPoint = locator->getPoint();

    float currTime = loopTimer->Get();

    std::vector<float> linearStep = linearProfile->getProfile(currTime);
    std::vector<float> angularStep = angularProfile->getProfile(currTime);

    float linearFF = (kLinVelFF*linearStep[2]) + (kLinAccelFF*linearStep[3]);
    float angularFF = (kAngVelFF*angularStep[2]) + (kAngAccelFF*angularStep[3]);

    relativeDistance = currPoint.distance - origPoint.distance;

    float baseError = angularProfile->getTarget() - currPoint.angle;

    float angleError = baseError;

    if ( fabs(relativeDistance - linearProfile->getTarget()) <= .5 && angleError <= 2 && locator->getLinearVelocity() < 2) {
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
    angularInput = (turnPID->update(locator->normalizeAngle(angleError), loopTimer)) + angularFF;

    if (driveInput > speedLimit) {
        driveInput = speedLimit;
    } else if (driveInput < -speedLimit) {
        driveInput = -speedLimit;
    }

    updateValue->throttle = driveInput;
    updateValue->turn = angularInput;
}
}
