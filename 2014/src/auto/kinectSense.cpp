#include "WPILib.h"
#include "kinectSense.hpp"
#include "../kinectHandler.hpp"
#include "../drive.hpp"
#include "../arm.hpp"
#include "../shooter.hpp"
#include "../intake.hpp"
#include "waitCommand.hpp"
#include "autoDriveCommand.hpp"
#include "sequentialCommand.hpp"
#include "turnCommand.hpp"
#include "armPresetCommand.hpp"
#include "linearDriveCommand.hpp"
#include "fireCommand.hpp"
#include "corralCommand.hpp"
#include <math.h>
#include <vector>

KinectSense::KinectSense(KinectHandler *kinect_, Drive *drive_, Shooter *shooter_, Arm *arm_, Intake *intake_, int autoMode_, std::string side_)
{
    drive = drive_;
    kinect = kinect_;
    shooter = shooter_;
    arm = arm_;
    intake = intake_;

    autoMode = autoMode_;
    movement = 0;

    sequence.push_back(new AutoWaitCommand(0));
    init = false;

    movementSelected = false;
    finalSequence = true;

    left = false;
    right = false;

    // Turn Distances
    jukeTurn = 5;
    centerTurn = 10;
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
                clear();
                sequence.push_back(new ArmPresetCommand(arm, SHOOTING, 0));
                sequence.push_back(new LinearDriveCommand(drive, 108, 0, false, 2));
                if (left)
                {
                    sequence.push_back(new TurnCommand(drive, centerTurn, 1, 2));
                    kinect->clearLastHand();
                }
                else
                {
                    sequence.push_back(new TurnCommand(drive, -centerTurn, 1, 2));
                    kinect->clearLastHand();
                }
                init = false;
                movementSelected = true;
            }

            if (left && !finalSequence)
            {
                clear();
                sequence.push_back(new TurnCommand(drive, centerTurn+3, 1, 2));
                sequence.push_back(new AutoWaitCommand(1));
                sequence.push_back(new FireCommand(shooter, 1.5));
                sequence.push_back(new AutoWaitCommand(10));
                kinect->clearLastHand();
                init = false;
                finalSequence = true;
            }
            else if (right && !finalSequence)
            {
                sequence.push_back(new TurnCommand(drive, -(centerTurn+3), 1, 2));
                sequence.push_back(new AutoWaitCommand(1));
                sequence.push_back(new FireCommand(shooter, 1.5));
                sequence.push_back(new AutoWaitCommand(10));
                kinect->clearLastHand();
                init = false;
                finalSequence = true;
            }

            break;

        case HOT_ONE_BALL_SIDE:

            if (!movementSelected)
            {
                clear();
                sequence.push_back(new ArmPresetCommand(arm, SHOOTING, 0));
                sequence.push_back(new LinearDriveCommand(drive, 108, 0, false, 2));
                if (kinect->getScheduledHand() == side)
                {
                    sequence.push_back(new AutoWaitCommand(0));
                    kinect->clearLastHand();
                }
                else
                {
                    sequence.push_back(new AutoWaitCommand(3));
                    kinect->clearLastHand();
                }
                init = false;
                movementSelected = true;
            }

            if (left && !finalSequence)
            {
                clear();
                sequence.push_back(new TurnCommand(drive, jukeTurn, 1, 2));
                sequence.push_back(new AutoWaitCommand(1));
                sequence.push_back(new FireCommand(shooter, 1.5));
                sequence.push_back(new AutoWaitCommand(10));
                kinect->clearLastHand();
                init = false;
                finalSequence = true;
            }
            else if (right && !finalSequence)
            {
                sequence.push_back(new TurnCommand(drive, -jukeTurn, 1, 2));
                sequence.push_back(new AutoWaitCommand(1));
                sequence.push_back(new FireCommand(shooter, 1.5));
                sequence.push_back(new AutoWaitCommand(10));
                kinect->clearLastHand();
                init = false;
                finalSequence = true;
            }
            else if (!finalSequence)
            {
                sequence.push_back(new AutoWaitCommand(1));
                sequence.push_back(new FireCommand(shooter, 1.5));
                sequence.push_back(new AutoWaitCommand(10));
                init = false;
                finalSequence = true;
            }

            break;

        case HOT_TWO_BALL_CENTER:

            if (!movementSelected)
            {
                sequence.push_back(new ArmPresetCommand(arm, SHOOTING, 0));
                sequence.push_back(new CorralCommand(intake, true));
                sequence.push_back(new LinearDriveCommand(drive, 120, 0, false, 6));
                init = false;
                movementSelected = true;
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
            if (movementSelected)
            {
                finalSequence = false;
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
