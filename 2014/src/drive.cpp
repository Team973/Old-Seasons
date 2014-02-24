#include "WPILib.h"
#include "drive.hpp"
#include <math.h>

Drive::Drive(Talon *leftDrive_, Talon *rightDrive_, Solenoid *shifters_, Solenoid *kickUp_, Encoder *leftEncoder_, Encoder *rightEncoder_)
{
    leftDrive = leftDrive_;
    rightDrive = rightDrive_;

    shifters = shifters_;
    kickUp = kickUp_;

    leftEncoder = leftEncoder_;
    rightEncoder = rightEncoder_;

    M_PI = 3.141592;

    quickStopAccumulator = 0;
    negInertiaAccumulator = 0;
    oldWheel = 0;
    leftDist = 0;
    rightDist = 0;
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
    leftDrive->Set(limit(left));
    rightDrive->Set(-limit(right));
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
    float diameter = 2.75;
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
    return normalizeAngle(0); //TODO(oliver): Add the new gyro get here
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

void Drive::update(double DriveX, double DriveY, bool gear, bool kick, bool quickTurn)
{
    //CheesyDrive(DriveY, DriveX, gear, quickTurn);
    arcade(-DriveY, -DriveX);
    setLowGear(gear);
    setKickUp(kick);
}

void Drive::dashboardUpdate()
{
    SmartDashboard::PutNumber("Left Encoder RAW Ticks", leftEncoder->Get());
    SmartDashboard::PutNumber("Right Encoder RAW Ticks", rightEncoder->Get());
}

void Drive::resetDrive()
{
    resetDriveEncoders();
}
