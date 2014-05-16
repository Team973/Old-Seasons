//This is the right one
#include "WPILib.h"
#include "trapProfile.hpp"
#include <math.h>

TrapProfile::TrapProfile(float xTarget, float vMax, float aMax, float dMax, int loopTime)
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
    int indexSize = ceil(t3/loopTime) + 1;

    float profile[indexSize][4];

#define T 0
#define X 1
#define V 2
#define A 3

    if ( x12 > 0)
    {
        for (int n=0; n<indexSize; n++)
        {
            float t = n*loopTime;

            if (t < t1)
            {
                profile[n][T] = t;
                profile[n][X] = (.5)*aMax*(t*t);
                profile[n][V] = aMax*t;
                profile[n][A] = aMax;
            }
            else if (t < t2)
            {
                profile[n][T] = t;
                profile[n][X] = x1 + vMax * (t - t1);
                profile[n][V] = vMax;
                profile[n][A] = 0;
            }
            else if (t < t3)
            {
                profile[n][T] = t;
                profile[n][X] = x2 + vMax * (t - t2) - (.5)*dMax*pow((t-t2), 2);
                profile[n][V] = vMax - dMax *(t-t2);
                profile[n][A] = -dMax;
            }
            else
            {
                profile[n][T] = t;
                profile[n][X] = xTarget;
                profile[n][V] = 0;
                profile[n][A] = 0;
            }
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
        float indexSize = ceil(t3/loopTime) + 1;
        for (int i=0; i<indexSize; i++)
        {
            float t = i*loopTime;

            if (t < t1)
            {
                profile[i][T] = t;
                profile[i][X] = (.5)*aMax*(t*t);
                profile[i][V] = aMax*t;
                profile[i][A] = aMax;
            }
            else if (t < t2)
            {
                profile[i][T] = t;
                profile[i][X] = x1 + vPeak * (t - t1) - (.5)*dMax*pow((t-t1), 2);
                profile[i][V] = vPeak - dMax * (t - t1);
                profile[i][A] = 0;
            }
            else
            {
                profile[i][T] = t;
                profile[i][X] = xTarget;
                profile[i][V] = 0;
                profile[i][A] = 0;
            }
        }
    }
}
