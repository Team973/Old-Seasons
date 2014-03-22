#include "WPILib.h"
#include "kinectJuke.hpp"
#include "../drive.hpp"
#include "turnCommand.hpp"
#include "waitCommand.hpp"
#include "../kinectHandler.hpp"

KinectJuke::KinectJuke(KinectHandler *kinect_, Drive *drive_, float timeout_)
{
    kinect = kinect_;
    drive = drive_;
    setTimeout(timeout_);
    cmd = new AutoWaitCommand(0);
    init = false;
}

void KinectJuke::Init()
{
    timer->Start();
    timer->Reset();
    drive->holdPosition(false, 0, 0, 0, 0);
}

bool KinectJuke::Run()
{
    if (timer->Get() >= timeout)
    {
        if (!init)
        {
            cmd->Init();
            init = true;
        }

        if (cmd->Run())
        {
            return true;
        }
    }
    else
    {
        if (kinect->getLeftHand())
        {
            cmd = new TurnCommand(drive, 30, 1);
        }

        if (kinect->getRightHand())
        {
            cmd = new TurnCommand(drive, -30, 1);
        }
    }
    return false;
}
