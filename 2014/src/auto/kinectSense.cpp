#include "WPILib.h"
#include "kinectSense.hpp"
#include "../kinectHandler.hpp"

KinectSense::KinectSense(KinectHandler *kinect_)
{
    kinect = kinect_;
}

void KinectSense::Init()
{
}

bool KinectSense::Run()
{
    return true;
}
