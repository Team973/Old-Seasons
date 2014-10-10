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

#endif
