#include "WPILib.h"
#include "drive.hpp"
#include <math.h>
#include "pid.hpp"

Drive::Drive(Talon *leftDrive_, Talon *rightDrive_, Solenoid *shifters_, Solenoid *kickUp_, Encoder *leftEncoder_, Encoder *rightEncoder_, Encoder *gyro_)
{
    leftDrive = leftDrive_;
    rightDrive = rightDrive_;

    shifters = shifters_;
    kickUp = kickUp_;

    leftEncoder = leftEncoder_;
    rightEncoder = rightEncoder_;

    gyro = gyro_;

    M_PI = 3.141592;

    positionPID = new PID(0.05, 0.001, 0);
    positionPID->setICap(0.3);
    positionPID->setBounds(-0.5, 0.5);
    positionPID->start();
    anglePID = new PID(0, 0, 0);//.1);
    anglePID->setBounds(-0.5, 0.5);
    anglePID->start();

    quickStopAccumulator = 0;
    negInertiaAccumulator = 0;
    oldWheel = 0;
    leftDist = 0;
    rightDist = 0;

    driveInput = 0;
    turnInput = 0;
    isHolding = false;
    drivePercision = 0;
    turnPercision = 0;
}

float Drive::getWaypoint(int dist)
{
    float point = 0;

    switch(dist)
    {
        case FAR:
            point = -6;
            break;
        case MID:
            point = -4;
            break;
        case CLOSE:
            point = -2;
            break;
    }

    return point;
}

float Drive::limit(float x)
{
    if (x > 1)
        return 1;
    else if (x < -1)
        return -1;

    return x;
}

void Drive::holdPosition(bool hold, float target, float drivePercision_, float turnPercision_)
{
    isHolding = hold;
    if (isHolding)
    {
        positionPID->setTarget(target);
        anglePID->setTarget(getGyroAngle());
    }
    if (!drivePercision_) {drivePercision = 2;}
    else {drivePercision = drivePercision_;}

    if (!turnPercision_) {turnPercision = 2;}
    else {turnPercision = turnPercision_;}
}

void Drive::setDriveMotors(float left, float right)
{
    leftDrive->Set(-limit(left));
    rightDrive->Set(limit(right));
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
    float diameter = 4.8;
    float encoderTicks = 360;
    float distancePerRevolution = M_PI * diameter;
    leftDist = (leftEncoder->Get() / encoderTicks) * distancePerRevolution;
    rightDist = (rightEncoder->Get() / encoderTicks) * distancePerRevolution;
    return (leftDist + rightDist)/2;
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
    return normalizeAngle(-gyro->Get() * (360 / ticks));
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
    if (x < 0)
        return -x*x;
    return x*x;
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
  float turnNonlinHigh = 0.9;
  float turnNonlinLow = 0.8;
  float negInertiaHigh = 0.5;
  float senseHigh = 1.2;
  float senseLow = 0.6;
  float senseCutoff = 0.1;
  float negInertiaLowMore = 2.5;
  float negInertiaLowLessExt = 5.0;
  float negInertiaLowLess = 3.0;
  float quickStopTimeConstant = 0.1;
  float quickStopLinearPowerThreshold = 0.2;
  float quickStopStickScalar = 5.0;

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
}

void Drive::update(double DriveX, double DriveY, bool gear, bool kick, bool quickTurn, bool isAuto)
{
    calculateDrive();

    if (isAuto)
    {
        arcade(DriveY, DriveX);
    }
    else
    {
        //CheesyDrive(DriveY, DriveX, gear, quickTurn);
        arcade(-DriveY, -DriveX);
    }
    setLowGear(gear);
    setKickUp(kick);

    storeDriveCalculations();
}

void Drive::positionUpdate()
{
    float linearError = fabs(positionPID->getTarget() - getWheelDistance());
    float angleError = fabs(anglePID->getTarget() - getGyroAngle());
    if (isHolding)
    {
        driveInput = positionPID->update(getWheelDistance());
        turnInput = anglePID->update(getGyroAngle());
        if (linearError > drivePercision || angleError > turnPercision)
            arcade(-driveInput, turnInput);
        else
            arcade(0, 0);
    }
}

void Drive::dashboardUpdate()
{
}

void Drive::resetDrive()
{
    resetDriveEncoders();
    resetGyro();
}
