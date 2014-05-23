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

    float profile[4];

#define T 0
#define X 1
#define V 2
#define A 3

    if ( x12 > 0)
    {
        float t = loopTime;

        if (t < t1)
        {
            profile[T] = t;
            profile[X] = (.5)*aMax*(t*t);
            profile[V] = aMax*t;
            profile[A] = aMax;
        }
        else if (t < t2)
        {
            profile[T] = t;
            profile[X] = x1 + vMax * (t - t1);
            profile[V] = vMax;
            profile[A] = 0;
        }
        else if (t < t3)
        {
            profile[T] = t;
            profile[X] = x2 + vMax * (t - t2) - (.5)*dMax*pow((t-t2), 2);
            profile[V] = vMax - dMax *(t-t2);
            profile[A] = -dMax;
        }
        else
        {
            profile[T] = t;
            profile[X] = xTarget;
            profile[V] = 0;
            profile[A] = 0;
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
            profile[T] = t;
            profile[X] = (.5)*aMax*(t*t);
            profile[V] = aMax*t;
            profile[A] = aMax;
        }
        else if (t < t2)
        {
            profile[T] = t;
            profile[X] = x1 + vPeak * (t - t1) - (.5)*dMax*pow((t-t1), 2);
            profile[V] = vPeak - dMax * (t - t1);
            profile[A] = 0;
        }
        else
        {
            profile[T] = t;
            profile[X] = xTarget;
            profile[V] = 0;
            profile[A] = 0;
        }
    }

    return *profile;
}
