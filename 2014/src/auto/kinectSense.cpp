#include "WPILib.h"
#include "kinectSense.hpp"
#include "../kinectHandler.hpp"
#include "../drive.hpp"
#include "waitCommand.hpp"
#include "autoDriveCommand.hpp"
#include "sequentialCommand.hpp"
#include "turnCommand.hpp"
#include "linearDriveCommand.hpp"
#include <math.h>
#include <vector>

KinectSense::KinectSense(KinectHandler *kinect_, Drive *drive_, int autoMode_, float timeout_, std::string side_, std::string lane_, bool doubleTime_)
{
    drive = drive_;
    kinect = kinect_;
    side = side_;
    lane = lane_;

    if (lane == "far")
    {
        movement = 6*12;
    }
    else if (lane == "mid")
    {
        movement = 4*12;
    }
    else if (lane == "close")
    {
        movement = 2*12;
    }

    setTimeout(timeout_);

    autoMode = autoMode_;
    movement = 0;

    sequence.push_back(new AutoWaitCommand(0));
    init = false;

    movementSelected = false;
    finalSequence = false;

    left = false;
    right = false;

    doubleTime = doubleTime_;

    // Turn Distances
    jukeTurn = 5;
    centerTurn = 7;
}

void KinectSense::clear()
{
    sequence.clear();
}

void KinectSense::Init()
{
    timer->Start();
    timer->Reset();
    kinect->clearLastHand();
}

bool KinectSense::Run()
{

    switch (autoMode)
    {
        case HOT_ONE_BALL_CENTER:

            if (!movementSelected)
            {
                if (left)
                {
                    clear();
                    sequence.push_back(new TurnCommand(drive, centerTurn, .9, 2));
                    sequence.push_back(new AutoWaitCommand(.5));
                    init = false;
                    movementSelected = true;
                }
                else
                {
                    clear();
                    sequence.push_back(new TurnCommand(drive, -centerTurn, .9, 2));
                    sequence.push_back(new AutoWaitCommand(.5));
                    init = false;
                    movementSelected = true;
                }
            }

            break;

        case HOT_ONE_BALL_SIDE:

            if (!movementSelected)
            {
                if ((left || right) || (left && right))
                {
                    clear();
                    sequence.push_back(new AutoWaitCommand(0));
                    init = false;
                    movementSelected = true;
                }
                else
                {
                    clear();
                    sequence.push_back(new AutoWaitCommand(4));
                    init = false;
                    movementSelected = true;
                }
            }

            break;

        case HOT_TWO_BALL_CENTER:

            if (!movementSelected && !doubleTime)
            {
                if (left)
                {
                    clear();
                    sequence.push_back(new TurnCommand(drive, centerTurn, .9, 2));
                    sequence.push_back(new AutoWaitCommand(.5));
                    init = false;
                    movementSelected = true;
                }
                else
                {
                    clear();
                    sequence.push_back(new TurnCommand(drive, -centerTurn, .9, 2));
                    sequence.push_back(new AutoWaitCommand(.5));
                    init = false;
                    movementSelected = true;
                }
            }
            else if (!movementSelected)
            {
                if (drive->getGyroAngle() > 0)
                {
                    clear();
                    sequence.push_back(new TurnCommand(drive, -centerTurn, .9, 2));
                    sequence.push_back(new AutoWaitCommand(.5));
                    init = false;
                    movementSelected = true;
                }
                else
                {
                    clear();
                    sequence.push_back(new TurnCommand(drive, centerTurn, .9, 2));
                    sequence.push_back(new AutoWaitCommand(.5));
                    init = false;
                    movementSelected = true;
                }
            }

            break;

        case HOT_ONE_BALL_LANES:

            if (left)
            {
                movement = movement;
            }
            else
            {
                movement = -movement;
            }

            if (!movementSelected)
            {
                sequence.push_back(new AutoDriveCommand(drive, movement, 108, false, 5, 2, 5));
                sequence.push_back(new TurnCommand(drive, 0, 1.5));
                sequence.push_back(new AutoWaitCommand(.5));
                init = false;
                movementSelected = true;
            }

            break;

        case HOT_TWO_BALL_LANES:

            if (left)
            {
                movement = movement;
            }
            else
            {
                movement = -movement;
            }

            if (!movementSelected)
            {
                sequence.push_back(new AutoDriveCommand(drive, movement, 120, false, 5, 2, 5));
                sequence.push_back(new TurnCommand(drive, 0, 1.5));
                sequence.push_back(new AutoWaitCommand(.5));
                init = false;
                movementSelected = true;
            }

            break;
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

    return false;
}
