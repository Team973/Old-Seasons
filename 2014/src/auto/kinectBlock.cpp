#include "WPILib.h"
#include "kinectBlock.hpp"
#include "../drive.hpp"
#include "../kinectHandler.hpp"
#include "sequentialCommand.hpp"
#include "linearProfileCommand.hpp"
#include "turnProfileCommand.hpp"
#include "waitCommand.hpp"
#include "../hellaBlocker.hpp"
#include <vector>
#include <math.h>

KinectBlock::KinectBlock(KinectHandler *kinect_, Drive *drive_, int autoMode_, bool Hot_, float distance_)
{
    kinect = kinect_;
    drive = drive_;
    autoMode = autoMode_;
    distance = distance_;
    Hot = Hot_;

    directionFlag = 1;

    hotTimer = new Timer();

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
    if (Hot && !goalSelected)
    {
        hotTimer->Start();
        if (kinect->goLeft())
        {
            directionFlag = -1;
            distance = fabs(distance);
            goalSelected = true;
        }
        else if (kinect->goRight())
        {
            directionFlag = 1;
            distance = fabs(distance);
            goalSelected = true;
        }

        if (hotTimer->Get() > 2)
        {
            directionFlag = 1;
            goalSelected = true;
        }
    }
    else
    {
        if (distance > 0)
            directionFlag = 1;
        else
            directionFlag = -1;
    }

    switch (autoMode)
    {
        case SIMPLE:

            if (goalSelected)
            {
                clear();
                sequence.push_back( new LinearProfileCommand(drive, distance, 15, 10, 15, drive->generateDistanceTime(distance)));
                init = false;
            }

            break;

        case B_90:

            if (goalSelected)
            {
                clear();
                sequence.push_back(new TurnProfileCommand(drive, 90*-directionFlag, 10000, 10000, 10000, 3));
                sequence.push_back(new AutoWaitCommand(.25));
                sequence.push_back(new LinearProfileCommand(drive, -distance, 15, 10, 15, drive->generateDistanceTime(distance)));
                init = false;
            }

            break;

        case LOW_GOAL:

            if (goalSelected)
            {
                clear();
                sequence.push_back(new TurnProfileCommand(drive, 75*-directionFlag, 10000, 10000, 10000, 2));
                sequence.push_back(new AutoWaitCommand(.25));
                sequence.push_back(new LinearProfileCommand(drive, -distance, 15, 10, 15, drive->generateDistanceTime(distance)));
                init = false;
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
                movement = 0;
                turn = 0;
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

            drive->arcade(movement, turn);
        }
    }

    // This purposly never returns true because we never want to stop denying points
    return false;
}
