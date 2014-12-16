#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <math.h>
#include <string>
#include <cctype>
#include <sstream>

namespace frc973 {

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

template<typename T>
inline std::string asString(T in)
{
    std::stringstream ss;
    ss << in;
    return ss.str();
}

template<class T, class R>
class Tuple
{
public:
    Tuple(T first_, R second_)
    {
        first = first_;
        second = second_;
    }
    T getFirst()
    {
        return first;
    }
    R getSecond()
    {
        return second;
    }
private:
    T first;
    R second;
};

}

#endif
