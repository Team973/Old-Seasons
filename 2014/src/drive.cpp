#include "WPILib.h"
#include "drive.hpp"
#include <math.h>
#include "pid.hpp"
#include "trapProfile.hpp"
#include "dataLog.hpp"

Drive::Drive(Talon *leftDrive_, Talon *rightDrive_, Solenoid *shifters_, Solenoid *kickUp_, Encoder *leftEncoder_, Encoder *rightEncoder_, Encoder *gyro_, Gyro *testGyro_)
{
    leftDrive = leftDrive_;
    rightDrive = rightDrive_;

    shifters = shifters_;
    kickUp = kickUp_;

    leftEncoder = leftEncoder_;
    rightEncoder = rightEncoder_;

    gyro = gyro_;
    testGyro = testGyro_;

    prevAngle = 0;

    M_PI = 3.141592;

    quickStopAccumulator = 0;
    negInertiaAccumulator = 0;
    oldWheel = 0;
    leftDist = 0;
    rightDist = 0;

    leftPower = 0;
    rightPower = 0;

    loopTimer = new Timer();
    loopTimer->Start();

    //drivePID = new PID(.015, 0, 0.08);
    drivePID = new PID(.5, 0.0, 0.0);
    drivePID->setICap(0);
    drivePID->start();
    //rotatePID = new PID(.005, 0, 0.01);
    rotatePID = new PID(0.005, 0.001, 0);
    rotatePID->setICap(0.1);
    rotatePID->start();

    doWeProfile = false;

    linearGenerator = new TrapProfile();
    angularGenerator = new TrapProfile();

    deadPID = false;

    drivePID->setBounds(-.9, .9);
    rotatePID->setBounds(-.9, .9);

    //XXX destroy us when you are done testing
    tuneStick = new Joystick(3);
    buttonTimer = new Timer();
    buttonTimer->Start();

    log = new DataLog();
    printTimer = new Timer();
    printTimer->Start();

    ds = DriverStation::GetInstance();
}

float Drive::generateDistanceTime(float x)
{
    float t = fabs((x - getWheelDistance()) * 0.034);

    if (t <= 0)
        return 0;
    else if (t > 6)
        return 6;
    else
        return t;
}

float Drive::limit(float x)
{
    if (x > 1)
        return 1;
    else if (x < -1)
        return -1;

    return x;
}

void Drive::setDriveMotors(float left, float right)
{
    leftPower = -limit(left);
    rightPower = limit(right);
}

float Drive::getLeftDrive()
{
    return leftEncoder->Get();
}

float Drive::getRightDrive()
{
    return rightEncoder->Get();
}

float Drive::getWheelDistance()
{
    float diameter = 3.89;
    float encoderTicks = 360;
    float distancePerRevolution = M_PI * diameter;
    leftDist = ((leftEncoder->Get() / encoderTicks) * distancePerRevolution)/12;
    rightDist = ((rightEncoder->Get() / encoderTicks) * distancePerRevolution)/12;
    return (leftDist + rightDist)/2;
}

float Drive::getVelocity()
{
    float diameter = 3.89;
    float leftVel = leftEncoder->GetRate() / 360 * M_PI / 12 * diameter;
    float rightVel = rightEncoder->GetRate() / 360 * M_PI / 12 * diameter;;
    return (leftVel + rightVel)/2;
}

float Drive::getLeftVelocity()
{
    float diameter = 3.89;
    return leftEncoder->GetRate() / 360 * M_PI / 12 * diameter;
}

float Drive::getRightVelocity()
{
    float diameter = 3.89;
    return rightEncoder->GetRate() / 360 * M_PI / 12 * diameter;
}

float Drive::normalizeAngle(float theta)
{
    while (theta > 180)
    {
        theta -= 360;
    }
    while (theta < -180)
    {
        theta += 360;
    }

    return theta;
}

float Drive::getGyroAngle()
{
    float ticks = 1024;
    return normalizeAngle(gyro->Get() * (360 / ticks));
}

void Drive::resetGyro()
{
    gyro->Reset();
}

void Drive::resetDriveEncoders()
{
    leftEncoder->Reset();
    rightEncoder->Reset();
}

float Drive::getLeftDistance()
{
    return leftDist;
}

float Drive::getRightDistance()
{
    return rightDist;
}

void Drive::calculateDrive()
{
    currGyro = getGyroAngle();
    currTheta = prevTheta + (currGyro - prevGyro);
    theta = (currTheta + prevTheta) / 2;
    currLeft =  getLeftDistance();
    currRight = getRightDistance();
    magnitude = (currLeft + currRight - prevLeft - prevRight) / 2;
    deltaX = -magnitude * sin(theta / 180 * M_PI);
    deltaY = magnitude * cos(theta / 180 * M_PI);
    currX = prevX + deltaX;
    currY = prevY + deltaY;
}

void Drive::storeDriveCalculations()
{
    prevGyro = currGyro;
    prevTheta = currTheta;
    prevLeft = currLeft;
    prevRight = currRight;
    prevX = currX;
    prevY = currY;
}

float Drive::getX()
{
    return currX;
}

float Drive::getY()
{
    return currY;
}

float Drive::signSquare(float x)
{
    /*
    if (x < 0)
        return -x*x;
    return x*x;
    */
    return x;
}

void Drive::arcade(float move_, float rotate_)
{
    float move = signSquare(limit(move_));
    float rotate = signSquare(limit(rotate_));
    float left, right;
    if (move < 0)
    {
        if (rotate > 0)
        {
            left = (-move - rotate);
            right = max(-move, rotate);
        }
        else
        {
            left = max(-move, -rotate);
            right = (-move + rotate);
        }
    }
    else
    {
        if (rotate > 0)
        {
            left = -max(move, rotate);
            right = (-move + rotate);
        }
        else
        {
            left = (-move - rotate);
            right = -max(move, -rotate);
        }
    }
    setDriveMotors(left, right);
}

void Drive::setLowGear(bool lowGear)
{
    shifters->Set(lowGear);
}

void Drive::setKickUp(bool kick)
{
    kickUp->Set(kick);
}

void Drive::CheesyDrive(double throttle, double wheel, bool highGear, bool quickTurn) {
  bool isQuickTurn = quickTurn;
  float turnNonlinHigh = 0.2; // smaller is more responsive bigger is less responsive
  float turnNonlinLow = 0.8;
  float negInertiaHigh = 1.2; // bigger is more responsive
  float senseHigh = 1.2;
  float senseLow = 0.6;
  float senseCutoff = 0.1;
  float negInertiaLowMore = 2.5;
  float negInertiaLowLessExt = 5.0;
  float negInertiaLowLess = 3.0;
  float quickStopTimeConstant = 0.1;
  float quickStopLinearPowerThreshold = 0.2;
  float quickStopStickScalar = 0.8; //Bigger for faster stopping

  double wheelNonLinearity;

  double negInertia = wheel - oldWheel;
  oldWheel = wheel;

  if (highGear) {
    wheelNonLinearity = turnNonlinHigh;
    // Apply a sin function that's scaled to make it feel better.
    wheel = sin(M_PI / 2.0 * wheelNonLinearity * wheel) / sin(M_PI / 2.0 * wheelNonLinearity);
    wheel = sin(M_PI / 2.0 * wheelNonLinearity * wheel) / sin(M_PI / 2.0 * wheelNonLinearity);
  } else {
    wheelNonLinearity = turnNonlinLow;
    // Apply a sin function that's scaled to make it feel better.
    wheel = sin(M_PI / 2.0 * wheelNonLinearity * wheel) / sin(M_PI / 2.0 * wheelNonLinearity);
    wheel = sin(M_PI / 2.0 * wheelNonLinearity * wheel) / sin(M_PI / 2.0 * wheelNonLinearity);
    wheel = sin(M_PI / 2.0 * wheelNonLinearity * wheel) / sin(M_PI / 2.0 * wheelNonLinearity);
  }

  double leftPwm, rightPwm, overPower;
  float sensitivity = 1.7;

  float angularPower;
  float linearPower;

  // Negative inertia!
  double negInertiaScalar;
  if (highGear) {
    negInertiaScalar = negInertiaHigh;
    sensitivity = senseHigh;
  } else {
    if (wheel * negInertia > 0) {
      negInertiaScalar = negInertiaLowMore;
    } else {
      if (fabs(wheel) > 0.65) {
        negInertiaScalar = negInertiaLowLessExt;
      } else {
        negInertiaScalar = negInertiaLowLess;
      }
    }
    sensitivity = senseLow;

    if (fabs(throttle) > senseCutoff) {
      sensitivity = 1 - (1 - sensitivity) / fabs(throttle);
    }
  }
  double negInertiaPower = negInertia * negInertiaScalar;
  negInertiaAccumulator += negInertiaPower;

  wheel = wheel + negInertiaAccumulator;
  if(negInertiaAccumulator > 1)
    negInertiaAccumulator -= 1;
  else if (negInertiaAccumulator < -1)
    negInertiaAccumulator += 1;
  else
    negInertiaAccumulator = 0;

  linearPower = throttle;

  // Quickturn!
  if (isQuickTurn) {
    if (fabs(linearPower) < quickStopLinearPowerThreshold) {
      double alpha = quickStopTimeConstant;
      quickStopAccumulator = (1 - alpha) * quickStopAccumulator + alpha * limit(wheel) * quickStopStickScalar;
    }
    overPower = 1.0;
    if (highGear) {
      sensitivity = 1.0;
    } else {
      sensitivity = 1.0;
    }
    angularPower = wheel;
  } else {
    overPower = 0.0;
    angularPower = fabs(throttle) * wheel * sensitivity - quickStopAccumulator;
    if (quickStopAccumulator > 1) {
      quickStopAccumulator -= 1;
    } else if (quickStopAccumulator < -1) {
      quickStopAccumulator += 1;
    } else {
      quickStopAccumulator = 0.0;
    }
  }

  rightPwm = leftPwm = linearPower;
  leftPwm += angularPower;
  rightPwm -= angularPower;

  if (leftPwm > 1.0) {
    rightPwm -= overPower * (leftPwm - 1.0);
    leftPwm = 1.0;
  } else if (rightPwm > 1.0) {
    leftPwm -= overPower * (rightPwm - 1.0);
    rightPwm = 1.0;
  } else if (leftPwm < -1.0) {
    rightPwm += overPower * (-1.0 - leftPwm);
    leftPwm = -1.0;
  } else if (rightPwm < -1.0) {
    leftPwm += overPower * (-1.0 - rightPwm);
    rightPwm = -1.0;
  }

  setDriveMotors(leftPwm, rightPwm);
  setLowGear(highGear);
}

void Drive::killPID(bool death)
{
    deadPID = death;
}

void Drive::setAngular(TrapProfile *angularGenerator_)
{
    drivePID->setGains(0,0,0);
    rotatePID->setGains(0.01,0.001,0);
    angularGenerator = angularGenerator_;
    loopTimer->Reset();
}

void Drive::setLinear(TrapProfile *linearGenerator_)
{
    drivePID->setGains(0.5,0,0);
    linearGenerator = linearGenerator_;
    doWeProfile = true;
    loopTimer->Reset();
}

void Drive::update(bool isAuto)
{
    float currAngle = getGyroAngle();
    //float kLinVelFF = 0.155;
    float kLinVelFF = 0.0;
    float kLinAccelFF = 0.0;
    if (doWeProfile)
    {
        kLinVelFF = 0.08;
        kLinAccelFF = 0.03;
    }
    float kAngVelFF = 0;
    float kAngAccelFF = 0;
    //float kDccellFF = 0;


    if (isAuto)
    {
        if (!deadPID)
        {
            float loopTime = loopTimer->Get();
            std::vector<float> linearStep = linearGenerator->getProfile(loopTime);
            std::vector<float> angularStep = angularGenerator->getProfile(loopTime);

            /*
            SmartDashboard::PutNumber("Velocity Error: ", linearStep[2] - getVelocity());
            SmartDashboard::PutNumber("Position Error: ", linearStep[1] - getWheelDistance());
            SmartDashboard::PutNumber("Angular Error: ", angularStep[1] - getGyroAngle());
            */


            float linearInput, angularInput;
            /*
            if (linearStep[3] < 0)
            {
                kLinAccelFF = kDccelFF;
            }
            */
            linearInput = -(kLinVelFF*linearStep[2]) + -(kLinAccelFF*linearStep[3]);
            angularInput = -(kAngVelFF*angularStep[2]) + -(kAngAccelFF*angularStep[3]);

            /*
            if (fabs(linearOutput) < .1 && linearOutput != 0)
            {
                linearOutput = .1 * (fabs(linearOutput)/linearOutput);
            }

            SmartDashboard::PutNumber("Linear Output: ", linearOutput);
            SmartDashboard::PutNumber("Velocity: ", linearStep[2]);
            SmartDashboard::PutNumber("Acceleration: ", linearStep[3]);
            */

            float linearOutput, angularOutput;

            linearOutput = drivePID->update(linearStep[1]-getWheelDistance(), loopTimer) + linearInput;
            angularOutput = -rotatePID->update(angularStep[1] - getGyroAngle(), loopTimer);


            std::string s = "---" + log->asString(getLeftDistance()) + ", " + log->asString(getLeftVelocity()) + ", " + log->asString(getRightDistance()) + ", " + log->asString(getRightVelocity()) + ", " + log->asString(getWheelDistance()) + ", " + log->asString(getVelocity()) + ", " + log->asString(getGyroAngle()) + ", " + log->asString(loopTime) + ", " + log->asString(ds->GetBatteryVoltage()) + ", " + log->asString(leftPower) + ", " + log->asString(rightPower) + ", " + log->asString(linearOutput) + ", " + log->asString(drivePID->getP()) + ", " + log->asString(drivePID->getI()) + ", " + log->asString(drivePID->getD()) + ", " + log->asString(-(kLinVelFF*linearStep[2])) + ", " + log->asString(-(kLinAccelFF*linearStep[3])) + ", " + log->asString(linearStep[1] - getWheelDistance()) + ", " + log->asString(linearStep[2] - getVelocity()) + ", " + log->asString(linearStep[1]) + ", " + log->asString(linearStep[2]) + ", " + log->asString(linearStep[3]) + "____";
                printf("%s\n", s.c_str());
                printTimer->Reset();

            //SmartDashboard::PutNumber("I Contribution: ", rotatePID->update(angularStep[1]-getGyroAngle(), loopTimer) - 0.01*(angularStep[1]-getGyroAngle()));
            //SmartDashboard::PutNumber("Linear Output: ", linearOutput);
            arcade(linearOutput, angularOutput);
        }
    }

    prevAngle = currAngle;

    leftDrive->Set(leftPower);
    rightDrive->Set(rightPower);
    // was taken off for chezy champs and is no longer needed
    //setKickUp(kick);
}

void Drive::dashboardUpdate()
{
}

void Drive::resetDrive()
{
    resetDriveEncoders();
    resetGyro();
}
