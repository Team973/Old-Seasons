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
    drive->holdPosition(false, 0, 0, 0);
}

bool KinectSense::Run()
{
    if (kinect->getLeftHand())
    {
        drive->holdPosition(false, 0, 0, 0);
        movement = -.5;
    }
    else if (kinect->getRightHand())
    {
        drive->holdPosition(false, 0, 0, 0);
        movement = .5;
    }
    else if (!kinect->getRightHand() && !kinect->getLeftHand())
    {
        drive->holdPosition(true, drive->getWheelDistance(), 4, 2);
    }

    drive->update(0, -movement, false, false, false, true);
    return false;
}
