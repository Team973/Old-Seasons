#include "WPILib.h"
#include "kinectJuke.hpp"
#include "../drive.hpp"
#include "../kinectHandler.hpp"
#include "sequentialCommand.hpp"
#include "waitCommand.hpp"
#include "turnCommand.hpp"
#include <vector>

KinectJuke::KinectJuke(KinectHandler *kinect_, Drive *drive_, float timeout_)
{
    kinect = kinect_;
    drive = drive_;

    sequence.push_back(new AutoWaitCommand(0)); //dummy command

    setTimeout(timeout_);

    left = false;
    right = false;

    movementSelected = false;

    init = false;
}

void KinectJuke::clear()
{
    sequence.clear();
}

void KinectJuke::Init()
{
    timer->Start();
    timer->Reset();
    kinect->clearLastHand();
}

bool KinectJuke::Run()
{
    if (!movementSelected)
    {
        if (left)
        {
            clear();
            sequence.push_back(new TurnCommand(drive, drive->getGyroAngle()+3, 1, 2));
            sequence.push_back(new AutoWaitCommand(.5));
            init = false;
            movementSelected = true;
        }
        else if (right)
        {
            clear();
            sequence.push_back(new TurnCommand(drive, drive->getGyroAngle()-3, 1, 2));
            sequence.push_back(new AutoWaitCommand(.5));
            init = false;
            movementSelected = true;
        }
        else
        {
            clear();
            sequence.push_back(new AutoWaitCommand(0));
            init = false;
            movementSelected = true;
        }
    }

    if (kinect->getScheduledHand() == "left" || kinect->getLeftHand())
    {
        left = true;
        right = false;
    }
    if (kinect->getScheduledHand() == "right" || kinect->getRightHand())
    {
        left = false;
        right = true;
    }

    if (timer->Get() >= timeout)
    {
        if (!init)
        {
            cmd = new SequentialCommand(sequence);
            cmd->Init();
            init = true;
        }
        else
        {
            if (cmd->Run())
            {
                return true;
            }
        }
    }

    return false;
}
