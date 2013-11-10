#include "WPILib.h"
#include "drive.hpp"
#include <math.h>

Drive::Drive()
{
    frontLeftDrive = new Talon(1);
    frontRightDrive = new Talon(2);
    backLeftDrive = new Talon(9);
    backRightDrive = new Talon(10);

    lowGear = new Solenoid(3);
    kickUp = new Solenoid(2);
    oldWheel = 0.0;
    negInertiaAccumulator = 0.0;
    quickStopAccumulator = 0.0;
    isLowGear = false;
    isKickUp = false;
}

void Drive::setKickUp(bool k)
{
    isKickUp = k;
}

// Divide by for 2.778 scaling
void Drive::setBackWheelsDown(bool d)
{
    isLowGear = d;
}

void Drive::setHighGear(bool g)
{
    if ((isLowGear) || (isKickUp))
    {
    isLowGear = isLowGear;
    isKickUp = isKickUp;
    }
    else
    {
        isLowGear = g;
        isKickUp = g;
    }
}

void Drive::setDriveMotors(float left, float right)
{
    if ((isKickUp) && (! isLowGear))
    {
        frontLeftDrive->Set(-limit(left));
        backLeftDrive->Set(-limit(left));

        frontRightDrive->Set(limit(right / 2.778));
        backRightDrive->Set(limit(right / 2.778));
    }
    else if ((! isKickUp) && (isLowGear))
    {
        frontLeftDrive->Set(-limit(left / 2.778));
        backLeftDrive->Set(-limit(left / 2.778));

        frontRightDrive->Set(limit(right));
        backRightDrive->Set(limit(right));
    }
    else
    {
        frontLeftDrive->Set(-limit(left));
        backLeftDrive->Set(-limit(left));

        frontRightDrive->Set(limit(right));
        backRightDrive->Set(limit(right));
    }

}

void Drive::setLeftDrive(float speed)
{
    frontLeftDrive->Set(-limit(speed));
    backLeftDrive->Set(-limit(speed));
}

void Drive::setRightDrive(float speed)
{
    frontRightDrive->Set(limit(speed));
    backRightDrive->Set(limit(speed));
}

void Drive::setFrontLeftDrive(float speed)
{
    frontLeftDrive->Set(speed);
}

void Drive::setBackLeftDrive(float speed)
{
    backLeftDrive->Set(speed);
}

void Drive::setFrontRightDrive(float speed)
{
    frontRightDrive->Set(speed);
}

void Drive::setBackRightDrive(float speed)
{
    backRightDrive->Set(speed);
}

float Drive::limit(float x)
{
    if (x > 1)
        return 1;
    else if (x < -1)
        return -1;
    else
        return x;
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

  double M_PI = 3.141592;

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
  static double negInertiaAccumulator = 0.0;
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
  setHighGear(highGear);
}

void Drive::update(double DriveX, double DriveY, bool Gear, bool quickTurn)
{
    if ((isLowGear) && (isKickUp))
    {
        CheesyDrive(DriveY, (DriveX * 0), Gear, false);
    }
    else
    {
        CheesyDrive(DriveY, DriveX, Gear, quickTurn);
    }

    lowGear->Set(isLowGear);
    kickUp->Set(isKickUp);
}

