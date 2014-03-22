#include "WPILib.h"
#include "kinectSense.hpp"
#include "../kinectHandler.hpp"
#include "../drive.hpp"
#include "../autoManager.hpp"

KinectSense::KinectSense(KinectHandler *kinect_, Drive *drive_, int autoMode_, float timeout_)
{
    drive = drive_;
    kinect = kinect_;

    autoMode = autoMode_;
    movement = 0;

    setTimeout(timeout_);
}

void KinectSense::Init()
{
    timer->Start();
    timer->Reset();

    drive->holdPosition(false, 0, 0, 0, 0);
}

bool KinectSense::Run()
{
    /*
    if (autoMode == (HELLAVATOR_FOREWARD || HELLAVATOR_BACKWARD))
    {
    */
        if (kinect->getLeftHand())
        {
            drive->holdPosition(false, 0, 0, 0, 0);
            movement = -.6;
        }
        else if (kinect->getRightHand())
        {
            drive->holdPosition(false, 0, 0, 0, 0);
            movement = .6;
        }
        else if (!kinect->getRightHand() && !kinect->getLeftHand())
        {
            drive->holdPosition(true, drive->getWheelDistance(), drive->getGyroAngle(), 2, 2);
        }
        drive->update(0, -movement, false, false, false, true);
    //}

    /*
    if (autoMode == (TWO_BALL))
    {
        if (kinect->getLeftHand())
        {
        }
        else if (kinect->getRightHand())
        {
        }
    }
    */
    return false;
}
