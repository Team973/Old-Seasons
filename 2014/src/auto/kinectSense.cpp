#include "WPILib.h"
#include "kinectSense.hpp"
#include "../kinectHandler.hpp"
#include "../drive.hpp"
#include "waitCommand.hpp"
#include "autoDriveCommand.hpp"
#include "sequentialCommand.hpp" // for later
#include "turnCommand.hpp"
#include <math.h>
#include <vector>

KinectSense::KinectSense(KinectHandler *kinect_, Drive *drive_, int autoMode_, bool instantExecution_, float timeout_)
{
    drive = drive_;
    kinect = kinect_;

    autoMode = autoMode_;
    movement = 0;

    instantExecution = instantExecution_;

    setTimeout(timeout_);

    sequence.push_back( new AutoDriveCommand(drive, drive->generateTurnWaypoint(), drive->getY()+12, drive->generateDriveTime()));
    sequence.push_back( new AutoDriveCommand(drive, drive->getWaypoint(), drive->getFinalY(), false, drive->generateDriveTime()));
    init = false;
}

void KinectSense::clear()
{
    sequence.clear();
}

void KinectSense::Init()
{
    timer->Start();
    timer->Reset();
}

bool KinectSense::Run()
{
    switch (autoMode)
    {
        case BLOCKER:

            if ((!kinect->getRightHand() && !kinect->getLeftHand()) || (kinect->getRightHand() && kinect->getLeftHand()))
            {
                if (drive->getWheelDistance() > 0)
                    movement = -.2;
                else if (drive->getWheelDistance() < 0)
                    movement = .2;
            }
            else if (kinect->getLeftHand())
            {
                movement = -.6;
            }
            else if (kinect->getRightHand())
            {
                movement = .6;
            }

            drive->update(0, -movement, false, false, false, true);

            // This purposly never returns true because we never want to stop denying points

            break;

        case MULTI_BALL:

            if (kinect->getLeftHand())
            {
                if (fabs(drive->getY() - drive->getFinalY()) < 5) // if we have reached our destination then let the juking begin
                {
                    clear();
                    sequence.push_back(new TurnCommand(drive, 30, .8));
                    init = false;
                }
                else
                {
                    drive->goLeft();
                }
            }
            else if (kinect->getRightHand())
            {
                if (fabs(drive->getY() - drive->getFinalY()) < 5)
                {
                    clear();
                    sequence.push_back(new TurnCommand(drive, -30, .8));
                    init = false;
                }
                else
                {
                    drive->goRight();
                }
            }

            if (timer->Get() > timeout || instantExecution)
            {
                if (!init)
                {
                    cmd = new SequentialCommand(sequence);
                    cmd->Init();
                    init = true;
                }

                if (cmd->Run())
                {
                    return true;
                }
            }

            break;
    }

    return false;
}
