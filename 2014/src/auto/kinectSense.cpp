#include "WPILib.h"
#include "kinectSense.hpp"
#include "../kinectHandler.hpp"
#include "../drive.hpp"
#include "waitCommand.hpp"
#include "sequentialCommand.hpp" // for later

KinectSense::KinectSense(KinectHandler *kinect_, Drive *drive_, int autoMode_, float timeout_)
{
    drive = drive_;
    kinect = kinect_;

    autoMode = autoMode_;
    movement = 0;

    setTimeout(timeout_);
    cmd = new AutoWaitCommand(0);
    init = false;
}

void KinectSense::Init()
{
    timer->Start();
    timer->Reset();

    drive->holdPosition(false, 0, 0, 0, 0);
}

bool KinectSense::Run()
{
    switch (autoMode)
    {
        case BLOCKER:

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

            // This purposly never returns true because we never want to stop denying points

            break;

        case MULTI_BALL:

            break;
    }

    return false;
}
