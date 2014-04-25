#include "WPILib.h"
#include "kinectBlock.hpp"
#include "../drive.hpp"
#include "../kinectHandler.hpp"
#include "sequentialCommand.hpp"
#include "linearDriveCommand.hpp"
#include "waitCommand.hpp"
#include "../hellaBlocker.hpp"
#include <vector>

KinectBlock::KinectBlock(KinectHandler *kinect_, Drive *drive_, HellaBlocker *blocker_, int autoMode_, float initialDistance_, float finalDistance_, float driveTime_)
{
    kinect = kinect_;
    drive = drive_;
    blocker = blocker_;
    autoMode = autoMode_;
    initialDistance = initialDistance_;
    finalDistance = finalDistance_;

    if (initialDistance > 0)
        autoAngle = 5;
    else
        autoAngle = -5;

    driveTime = driveTime_;

    movement = false;
    goalSelected = false;
    init = false;

    turn = 0;

    sequence.push_back(new AutoWaitCommand(0)); // pass a dummy command
}

void KinectBlock::clear()
{
    sequence.clear();
}

void KinectBlock::Init()
{
    clear();
    kinect->clearLastHand();
}

bool KinectBlock::Run()
{
    /*
    if (autoTimer->Get() >= driveTime)
    {
        if (finalDistance > 0)
            autoAngle = 5;
        else
            autoAngle = -5;
    }
    */

    switch (autoMode)
    {
        case SIMPLE:

            if (!goalSelected)
            {
                clear();
                sequence.push_back( new LinearDriveCommand(drive, initialDistance, 0, false, drive->generateDistanceTime(initialDistance)));
                init = false;
                goalSelected = true;
            }

            break;

        case HOT_SIMPLE:

            if (!goalSelected)
            {
                if (kinect->getScheduledHand() == "left" || kinect->getLeftHand())
                {
                    clear();
                    sequence.push_back( new LinearDriveCommand(drive, initialDistance, 5, false, drive->generateDistanceTime(initialDistance)));
                    init = false;
                    goalSelected = true;
                }
                else if (kinect->getScheduledHand() == "right" || kinect->getRightHand())
                {
                    clear();
                    sequence.push_back( new LinearDriveCommand(drive, -initialDistance, -5, false, drive->generateDistanceTime(initialDistance)));
                    init = false;
                    goalSelected = true;
                }
            }

            break;

        case DOUBLE_TROUBLE:

            if (autoTimer->Get() >= driveTime)
            {
                goalSelected = false;
            }

            if (!goalSelected && autoTimer->Get() >= driveTime)
            {
                clear();
                sequence.push_back( new LinearDriveCommand(drive, finalDistance, autoAngle, false, drive->generateDistanceTime(finalDistance)));
                init = false;
                goalSelected = true;
            }
            else if (!goalSelected)
            {
                clear();
                sequence.push_back( new LinearDriveCommand(drive, initialDistance, autoAngle, false, drive->generateDistanceTime(initialDistance)));
                init = false;
                goalSelected = true;
            }

            break;

        case HOT_DOUBLE_TROUBLE:

            if (!goalSelected && autoTimer->Get() >= driveTime)
            {
                clear();
                sequence.push_back( new LinearDriveCommand(drive, finalDistance, autoAngle, false, drive->generateDistanceTime(finalDistance)));
                init = false;
                goalSelected = true;
            }
            else if (!goalSelected)
            {
                if (kinect->getScheduledHand() == "left" || kinect->getLeftHand())
                {
                    clear();
                    sequence.push_back( new LinearDriveCommand(drive, initialDistance, 5, false, drive->generateDistanceTime(initialDistance)));
                    init = false;
                    goalSelected = true;
                }
                else if (kinect->getScheduledHand() == "right" || kinect->getRightHand())
                {
                    clear();
                    sequence.push_back( new LinearDriveCommand(drive, -initialDistance, -5, false, drive->generateDistanceTime(initialDistance)));
                    init = false;
                    goalSelected = true;
                }
            }

            break;

        case LOW_GOAL:

            if (initialDistance > 0)
            {
                blocker->front();
            }
            else
            {
                blocker->back();
            }

            if (!goalSelected)
            {
                clear();
                sequence.push_back( new LinearDriveCommand(drive, initialDistance, autoAngle, false, 5, BLOCK));
                init = false;
                goalSelected = true;
            }

            break;
    }

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
            drive->killPID(true);

            if (kinect->getRightHand() && kinect->getLeftHand())
            {
                if (autoMode == LOW_GOAL)
                {
                    if (drive->getWheelDistance() > 0)
                    {
                        turn = -.4;
                    }
                    else
                    {
                        turn = .4;
                    }
                }
                else
                {
                    movement = 0;
                    turn = 0;
                }
            }
            else if ((!kinect->getRightHand() && !kinect->getLeftHand()))
            {
                movement = 0;
                turn = 0;
            }
            else if (kinect->getLeftHand())
            {
                movement = -.6;
                turn = 0;
            }
            else if (kinect->getRightHand())
            {
                movement = .6;
                turn = 0;
            }

            drive->update(turn, -movement, false, false, false, true);
        }
    }

    // This purposly never returns true because we never want to stop denying points
    return false;
}
