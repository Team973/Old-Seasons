#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <math.h>
#include <string>
#include <cctype>
#include <sstream>
#include <vector>

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

inline std::string scrape(std::string line, char token)
{
    unsigned long point = line.find(token);
    if (point == std::string::npos)
    {
        return line;
    }
    //printf("preparing to scrape file\n");
    while ((point = line.find(token)) != std::string::npos)
    {
        line.erase(line.begin()+point);
    }
    //printf("file scraped\n");
    return line;
}

inline std::vector<std::string> split(std::string line, char token, int lineNumber)
{
    std::string str;
    std::vector<std::string> chunks;
    unsigned long point = line.find(token);
    int n = 0;
    
    if (point == std::string::npos)
    {
        printf("The line %i does not contain the character '%c'\n", lineNumber, token);
        str.append(line);
        chunks.push_back(str);
        return chunks;
    }
    
    // this loop removes the first part of the string
    //printf("preparing to split the file\n");
    while ((point = line.find(token)) != std::string::npos)
    {
        str.append(line, 0, point);
        line.erase(line.begin(), line.begin()+point+1);
        chunks.push_back(str);
        str.erase();
        n += 1;
    }
    //printf("file split\n");
    
    str.append(line);
    chunks.push_back(str);
    return chunks;
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
