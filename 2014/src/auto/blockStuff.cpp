#include "WPILib.h"
#include "blockStuff.hpp"
#include "linearProfileCommand.hpp"
#include "turnProfileCommand.hpp"
#include "sequentialCommand.hpp"
#include "waitCommand.hpp"
#include "../drive.hpp"
#include "../trapProfile.hpp"
#include "../kinectHandler.hpp"
#include <math.h>
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
    sequence.clear();
    hotTimer->Reset();
}

bool BlockStuff::Run()
{
    hotTimer->Start();

    if (kinect->getLeftHand() || kinect->getRightHand())
    {
        drive->killPID(true);
        kinectOver = true;
    }

    if (hot && !directionSelected)
    {
        if (hotTimer->Get() > 2)
        {
            directionSelected = true;
        }

        if (kinect->goLeft())
        {
            directionFlag = -1;
            directionSelected = true;
        }
        else if (kinect->goRight())
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
                sequence.push_back(new LinearProfileCommand(drive, fabs(distance)*directionFlag, 15, 10, 15, 5));
                init = false;
                generated = true;
                break;
            case B_90:
                sequence.clear();
                sequence.push_back(new TurnProfileCommand(drive, 90*-directionFlag, 100000, 100000, 100000, 3));
                sequence.push_back(new AutoWaitCommand(1));
                sequence.push_back(new LinearProfileCommand(drive, -distance*directionFlag, 15, 10, 15, 5));
                init = false;
                generated = true;
                break;
            case B_LOW:
                sequence.clear();
                sequence.push_back(new TurnProfileCommand(drive, 78*-directionFlag, 100000, 100000, 100000, 3));
                sequence.push_back(new AutoWaitCommand(1));
                sequence.push_back(new LinearProfileCommand(drive, -9, 15, 10, 15, 5));
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
    else if (!kinectOver && generated)
    {
        if (cmd->Run())
            kinectOver = true;
    }
    else if (kinectOver)
    {
        float movement = 0;
        if (kinect->getLeftHand())
            movement = .36;
        else if (kinect->getRightHand())
            movement = -.36;
        else
            movement = 0;

        if (mode != B_SIMPLE)
            movement *= -directionFlag;

        drive->arcade(movement, 0);
    }

    return false;
}
