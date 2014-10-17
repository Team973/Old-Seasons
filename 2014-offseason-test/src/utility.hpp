#include <math.h>

#ifndef UTILITY_HPP
#define UTILITY_HPP

float limit(float x)
{
    if (x > 1)
        return 1;
    else if (x < -1)
        return -1;
    else
        return x;
}

float deadband(float x, float threshold)
{
    if (fabs(x) < threshold)
        return 0;
    else
        return x;
}

#endif
