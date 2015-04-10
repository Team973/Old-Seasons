#include "WPILib.h"
#include "drive.hpp"
#include "xyManager.hpp"
#include <cmath>
#include "../lib/utility.hpp"

namespace frc973 {

Drive::Drive(VictorSP *left_, VictorSP *right_)
{
    leftMotor = left_;
    rightMotor = right_;

    xyManager = XYManager::getInstance();

    locked = false;

    oldWheel = 0;
    negInertiaAccumulator = 0;
    quickStopAccumulator = 0;
}

void Drive::lock() {
    locked = true;
}

void Drive::unlock() {
    locked = false;
}

bool Drive::isLocked() {
    return locked;
}

void Drive::setDriveMotors(float left, float right)
{
    if (locked && left < 0 && -right < 0) {
        left = 0;
        right = 0;
    }

    leftMotor->Set(limit(left));
    rightMotor->Set(-limit(right));

    SmartDashboard::PutNumber("left drive output: ", limit(left));
    SmartDashboard::PutNumber("right drive output: ", -limit(right));
}

void Drive::arcade(float move_, float rotate_)
{
    float move = limit(move_);
    float rotate = limit(rotate_);
    float left, right;
    if (move < 0)
    {
        if (rotate > 0)
        {
            left = (-move - rotate);
            right = fmax(-move, rotate);
        }
        else
        {
            left = fmax(-move, -rotate);
            right = (-move + rotate);
        }
    }
    else
    {
        if (rotate > 0)
        {
            left = -fmax(move, rotate);
            right = (-move + rotate);
        }
        else
        {
            left = (-move - rotate);
            right = -fmax(move, -rotate);
        }
    }
    setDriveMotors(left, right);
}

float Drive::signSquare(float x) {
    if (x < 0) {
        return -x*x;
    }
    return x*x;
}

void Drive::controlInterface(double throttle, double wheel, bool lowGear) {
    float turnCap, driveCap;
    if (lowGear) {
        driveCap = 0.3;
        turnCap = 0.5;
    } else {
        driveCap = 0.4;
        turnCap = 0.7;
    }

    double move = signSquare(throttle)*driveCap;
    double rotate = signSquare(wheel)*turnCap;
    if (fabs(move) > driveCap) {
        if (move < 0) {
            move = -driveCap;
        } else {
            move = driveCap;
        }
    }
    if (fabs(rotate) > turnCap) {
        if (rotate < 0) {
            rotate = -turnCap;
        } else {
            rotate = turnCap;
        }
    }
    arcade(move, rotate);
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
}

// This function should only be called in autonomous periodic
// if it is called during teleop the robot will revert to auto control
void Drive::update()
{
    XYManager::MotorValue* values = xyManager->getValues();
    arcade(values->throttle, -values->turn);
}

}
