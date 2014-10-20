#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <math.h>
#include <string>
#include <cctype>

inline float limit(float x)
{
    if (x > 1)
        return 1;
    else if (x < -1)
        return -1;
    else
        return x;
}

inline float deadband(float x, float threshold)
{
    if (fabs(x) < threshold)
        return 0;
    else
        return x;
}

inline std::string asLower(std::string line)
{
    for (unsigned int n=0;n<line.size();n++)
    {
        line[n] = tolower(line[n]);
    }

    return line;
}

#endif
