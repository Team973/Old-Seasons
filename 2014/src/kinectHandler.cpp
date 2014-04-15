#include "WPILib.h"
#include "kinectHandler.hpp"

KinectHandler::KinectHandler(KinectStick *left_, KinectStick *right_)
{
    left = left_;
    right = right_;

    lastHand = "none";
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

std::string KinectHandler::getScheduledHand()
{
    return lastHand;
}

bool KinectHandler::getLeftHand()
{
    lastHand = "left";
    return (kinectDeadband(left->GetY(), 0.5));
}

bool KinectHandler::getRightHand()
{
    lastHand = "right";
    return (kinectDeadband(right->GetY(), 0.5));
}
