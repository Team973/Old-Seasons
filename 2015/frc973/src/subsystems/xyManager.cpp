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
    angleTarget = 0;

    relativeDistance = 0;

    isPaused = true;

    speedLimit = 0.6;

    angularMovement = false;
    linearMovement = false;

    printf("Profile Pos, Profile Vel, Profile Accel, Actual Pos, Actual Vel\n");
}

void XYManager::resetEncoders() {
    locator->resetEncoders();
}

void XYManager::setSpeed(bool fast) {
    if (fast) {
        speedLimit = 0.3;
    } else {
        speedLimit = 0.18;
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
    angleTarget = currPoint.angle;
    drivePID->setBounds(-.6,.6);
    done = false;
    linearMovement = true;
    angularMovement = false;
}

void XYManager::setTargetAngle(float angle_)
{
    currPoint = locator->getPoint();
    angleTarget = angle_;
    drivePID->setBounds(0.0,0.0);
    done = false;
    linearMovement = false;
    angularMovement = true;
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

    float angleError = angleTarget - currPoint.angle;
    SmartDashboard::PutNumber("currpoint angle: ", currPoint.angle);
    SmartDashboard::PutNumber("curr angle error: ", angleError);
    /*
    if (fabs(turnPID->getTarget() - currPoint.angle) > fabs(turnPID->getTarget() + currPoint.angle)) {
        angleError = turnPID->getTarget() + currPoint.angle;
    } else {
        angleError = turnPID->getTarget() - currPoint.angle;
    }
    */

    float linearError = linearProfile->getTarget() - relativeDistance;
    if (linearMovement) {
        if (linearProfile->getTarget() < relativeDistance ? linearError >= -0.05 : linearError <= 0.5
                && fabs(locator->getLinearVelocity()) < 2 && fabs(angleError) <= 5) {
            done = true;
        }
    } else if (angularMovement) {
        if (fabs(angleError) <= 3) {
            done = true;
        }
    }

    float driveInput = 0;
    float angularInput = 0;

    SmartDashboard::PutNumber("Target Angle: ", turnPID->getTarget());

    driveInput = drivePID->update(relativeDistance - linearStep[1], loopTimer) + linearFF;
    angularInput = (turnPID->update(angleError, loopTimer));

    if (!isPaused && (linearProfile->getTarget() - relativeDistance) > 0) {
        driveInput += 0.1;
    }

    driveInput += 0.05*(linearProfile->getTarget() < 0 ? -1 : 1);
    if (driveInput > speedLimit) {
        driveInput = speedLimit;
    } else if (driveInput < -speedLimit) {
        driveInput = -speedLimit;
    }

    updateValue->throttle = driveInput;
    updateValue->turn = angularInput;
}
}
