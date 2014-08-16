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

bool KinectHandler::override()
{
    if (getLeftHand() && getRightHand())
        return true;
    else
        return false;
}

std::string KinectHandler::getScheduledHand()
{
    return lastHand;
}

bool KinectHandler::getLeftHand()
{
    return (kinectDeadband(left->GetY(), 0.5));
}

bool KinectHandler::getRightHand()
{
    return (kinectDeadband(right->GetY(), 0.5));
}

void KinectHandler::clearLastHand()
{
    lastHand = "none";
}

void KinectHandler::update()
{
    if (getLeftHand())
    {
        lastHand = "left";
    }
    else if (getRightHand())
    {
        lastHand = "right";
    }
}
