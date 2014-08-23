#include "WPILib.h"
#include <math.h>
#include "kinectHandler.hpp"

KinectHandler::KinectHandler(KinectStick *left_, KinectStick *right_)
{
    left = left_;
    right = right_;

    lastHand = "none";
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
    return left->GetY() < -0.5;
}

bool KinectHandler::getRightHand()
{
    return right->GetY() < -0.5;
}

void KinectHandler::clearLastHand()
{
    lastHand = "none";
}

bool KinectHandler::goLeft()
{
    return left->GetY() > .5;
}

bool KinectHandler::goRight()
{
    return right->GetY() > .5;
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
