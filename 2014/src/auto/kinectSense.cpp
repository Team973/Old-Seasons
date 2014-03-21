#include "WPILib.h"
#include "kinectSense.hpp"
#include "../kinectHandler.hpp"
#include "../drive.hpp"

KinectSense::KinectSense(KinectHandler *kinect_, Drive *drive_)
{
    drive = drive_;
    kinect = kinect_;

    movement = 0;
}

void KinectSense::Init()
{
}

bool KinectSense::Run()
{
    if (kinect->getLeftHand())
    {
        movement = -1;
    }
    else if (kinect->getRightHand())
    {
        movement = 1;
    }
    else
    {
        drive->holdPosition(true, drive->getWheelDistance(), 4, 2);
    }

    drive->update(0, movement, false, false, false, true);
    return false;
}
