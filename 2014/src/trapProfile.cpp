#include "WPILib.h"
#include "trapProfile.hpp"
#include <math.h>

TrapProfile::TrapProfile(float xTarget, float vMax, float aMax, float dMax, float loopTime)
{
    float t1 = vMax/aMax;
    float t23 = vMax/dMax;
    float x1 = (1/2)*aMax*(t1*t1);
    float x23 = (1/2)*dMax*(t23*t23);
    float x12 = xTarget - x1 - x23;
    float t12 = x12/vMax;
    float t2 = t1 + t12;
    float t3 = t2 + t23;
    float indexSize = ciel(t3/loopTime) + 1;

    float profile[indexSize][4];

#define T 0
#define X 1
#define V 2
#define A 3

    for (int n=0; n<indexSize; n++)
    {
        float t = n*loopTime;

        if (t < t1)
        {
            profile[n][T] = t;
            profile[n][X] = (1/2)*aMax*(t*t);
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
            profile[n][X] = x2 + vMax * (t - t2) - (1/2)*dMax*pow((t-t2), 2);
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
