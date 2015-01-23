#include "xyManager.hpp"
#include "WPILib.h"
#include "../lib/trapProfile.hpp"
#include "../lib/pid.hpp"
#include "lib/utility.hpp"
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

    updateValue = new XYManager::MotorValue;

    locator = NULL;

    linearProfile = new TrapProfile();
    angularProfile = new TrapProfile();

    loopTimer = new Timer();

    drivePID = new PID(
            Constants::getConstant("kDriveP")->getDouble(),
            Constants::getConstant("kDriveI")->getDouble(),
            Constants::getConstant("kDriveD")->getDouble());
    drivePID->start();
    drivePID->setBounds(-1,1);
    turnPID = new PID(
            Constants::getConstant("kTurnP")->getDouble(),
            Constants::getConstant("kTurnI")->getDouble(),
            Constants::getConstant("kTurnD")->getDouble());
    turnPID->start();
    turnPID->setBounds(-.7,7);

    printf("Profile Pos, Profile Vel, Profile Accel, Actual Pos, Actual Vel\n");
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
    angularProfile = new TrapProfile(currPoint.angle, 100000, 100000,10000); // this is purposfully blown up do not change the numbers
    done = false;
}

void XYManager::setTargetAngle(float angle_)
{
    angularProfile = new TrapProfile(angle_, 100000, 100000,10000); // this is purposfully blown up do not change the numbers
    linearProfile = new TrapProfile(0, 0, 0, 0); // this means that we don't have to seperate turn and drive in update
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
    locator->update();
    
    currPoint = locator->getPoint();

    float currTime = loopTimer->Get();

    std::vector<float> linearStep = linearProfile->getProfile(currTime);
    std::vector<float> angularStep = angularProfile->getProfile(currTime);

    float linearFF = (kLinVelFF*linearStep[2]) + (kLinAccelFF*linearStep[3]);
    float angularFF = (kAngVelFF*angularStep[2]) + (kAngAccelFF*angularStep[3]);

    float relativeDistance = currPoint.distance - origPoint.distance;
    float relativeAngle = currPoint.angle - origPoint.distance;

    float angleError = 0;
    if (fabs(currPoint.angle - angularStep[1]) > (currPoint.angle + angularStep[1])) {
        angleError = (currPoint.angle + angularStep[1]);
    }
    else {
        angleError = currPoint.angle - angularStep[1];
    }

    if (relativeDistance<= .5 && relativeAngle <= 2)
    {
        done = true;
    }
    else
    {
        done = false;
    }

    float driveInput, angularInput;

    SmartDashboard::PutString("DB/String 5", asString(relativeDistance));
    SmartDashboard::PutString("DB/String 6", asString(linearStep[1]));
    SmartDashboard::PutString("DB/String 7", asString(currPoint.distance));
    SmartDashboard::PutString("DB/String 8", asString(origPoint.distance));

    printf("%f, %f, %f, %f, %f\n", linearStep[1], linearStep[2], linearStep[3], relativeDistance, locator->getLinearVelocity());

    driveInput = drivePID->update(relativeDistance - linearStep[1], loopTimer) + linearFF;
    angularInput = turnPID->update(angleError, loopTimer) + angularFF;

    updateValue->throttle = driveInput;
    updateValue->turn = angularInput;
}

}
