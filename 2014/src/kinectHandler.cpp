#include "WPILib.h"
#include "kinectHandler.hpp"

KinectHandler::KinectHandler(KinectStick *left_, KinectStick *right_)
{
    left = left_;
    right = right_;
}

float KinectHandler::kinectDeadband(float x, float limit)
{
    if (x > limit)
    {
        return false;
    }
    else
        return true;
}

bool KinectHandler::getLeftHand()
{
    return (kinectDeadband(left->GetY(), 0.5));
}

bool KinectHandler::getRightHand()
{
    return (kinectDeadband(right->GetY(), 0.5));
}
