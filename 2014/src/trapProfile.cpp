//This is the right one
#include "WPILib.h"
#include "trapProfile.hpp"
#include <math.h>

TrapProfile::TrapProfile(float xTarget_, float vMax_, float aMax_, float dMax_)
{
    xTarget = xTarget_;
    vMax = vMax_;
    aMax = aMax_;
    dMax = dMax_;
}

float TrapProfile::getProfile(float loopTime)
{
    float t1 = vMax/aMax;
    float t23 = vMax/dMax;
    float x1 = (.5)*aMax*(t1*t1);
    float x23 = (.5)*dMax*(t23*t23);
    float x12 = xTarget - x1 - x23;
    float t12 = x12/vMax;
    float x2 = x1 + x12;
    float t2 = t1 + t12;
    float t3 = t2 + t23;
    int size = 1;

    float profile[size][4];

#define T 0
#define X 1
#define V 2
#define A 3

    if ( x12 > 0)
    {
        float t = loopTime;

        if (t < t1)
        {
            profile[size][T] = t;
            profile[size][X] = (.5)*aMax*(t*t);
            profile[size][V] = aMax*t;
            profile[size][A] = aMax;
        }
        else if (t < t2)
        {
            profile[size][T] = t;
            profile[size][X] = x1 + vMax * (t - t1);
            profile[size][V] = vMax;
            profile[size][A] = 0;
        }
        else if (t < t3)
        {
            profile[size][T] = t;
            profile[size][X] = x2 + vMax * (t - t2) - (.5)*dMax*pow((t-t2), 2);
            profile[size][V] = vMax - dMax *(t-t2);
            profile[size][A] = -dMax;
        }
        else
        {
            profile[size][T] = t;
            profile[size][X] = xTarget;
            profile[size][V] = 0;
            profile[size][A] = 0;
        }
    }
    else
    {
        float vPeak = sqrt((2*xTarget)/(1/aMax+1/dMax));
        x1 = .5*(vPeak*vPeak)/aMax;
        x12 = .5*(vPeak*vPeak)/dMax;
        t1 = vPeak/aMax;
        t12 = vPeak/dMax;
        t2 = t1 + t12;
        float t = loopTime;

        if (t < t1)
        {
            profile[size][T] = t;
            profile[size][X] = (.5)*aMax*(t*t);
            profile[size][V] = aMax*t;
            profile[size][A] = aMax;
        }
        else if (t < t2)
        {
            profile[size][T] = t;
            profile[size][X] = x1 + vPeak * (t - t1) - (.5)*dMax*pow((t-t1), 2);
            profile[size][V] = vPeak - dMax * (t - t1);
            profile[size][A] = 0;
        }
        else
        {
            profile[size][T] = t;
            profile[size][X] = xTarget;
            profile[size][V] = 0;
            profile[size][A] = 0;
        }
    }

    return profile;
}
