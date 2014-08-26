#include "WPILib.h"
#include "blockStuff.hpp"
#include "linearProfileCommand.hpp"
#include "turnProfileCommand.hpp"
#include "sequentialCommand.hpp"
#include "waitCommand.hpp"
#include "../drive.hpp"
#include "../trapProfile.hpp"
#include "../kinectHandler.hpp"
#include <vector>

BlockStuff::BlockStuff(Drive *drive_, KinectHandler *kinect_, float distance_, int mode_, bool hot_)
{
    drive = drive_;
    kinect = kinect_;
    distance = distance_;
    mode = mode_;
    hot = hot_;

    hotTimer = new Timer();

    generated = false;
    directionSelected = false;
    kinectOver = false;

    directionFlag = 1;

    init = false;
}

void BlockStuff::Init()
{
    hotTimer->Start();
    hotTimer->Reset();
}

bool BlockStuff::Run()
{
    if (kinect->getLeftHand() || kinect->getRightHand())
    {
        drive->killPID(true);
        kinectOver = true;
    }

    if (hot && !directionSelected)
    {
        if (kinect->goLeft())
        {
            directionFlag = -1;
            directionSelected = true;
        }
        else if (kinect->goRight() || hotTimer->Get())
        {
            directionFlag = 1;
            directionSelected = true;
        }
    }
    else if (!directionSelected)
    {
        if (distance > 0)
        {
            directionFlag = 1;
            directionSelected = true;
        }
        else
        {
            directionFlag = -1;
            directionSelected = true;
        }
    }

    if (!generated && !kinectOver && directionSelected)
    {
        switch (mode)
        {
            case B_SIMPLE:
                sequence.clear();
                sequence.push_back(new LinearProfileCommand(drive, distance*directionFlag, 15, 10, 15, 5));
                init = false;
                generated = true;
                break;
            case B_90:
                sequence.clear();
                sequence.push_back(new TurnProfileCommand(drive, 90*directionFlag, 100000, 100000, 100000, 3));
                sequence.push_back(new AutoWaitCommand(.25));
                sequence.push_back(new LinearProfileCommand(drive, -distance, 15, 10, 15, 5));
                init = false;
                generated = true;
                break;
            case B_LOW:
                sequence.clear();
                sequence.push_back(new TurnProfileCommand(drive, 75*directionFlag, 100000, 100000, 100000, 3));
                sequence.push_back(new AutoWaitCommand(.25));
                sequence.push_back(new LinearProfileCommand(drive, -10, 15, 10, 15, 5));
                init = false;
                generated = true;
                break;
        }
    }

    if (!init && generated)
    {
        cmd = new SequentialCommand(sequence);
        cmd->Init();
        init = true;
    }
    else if (!kinectOver)
    {
        if (cmd->Run())
            kinectOver = true;
    }
    else
    {
        float movement = 0;
        if (kinect->getLeftHand())
            movement = -.6;
        else if (kinect->getLeftHand())
            movement = .6;
        else
            movement = 0;

        if (mode != B_SIMPLE)
            movement *= directionFlag;

        drive->arcade(movement, 0);
    }

    return false;
}
