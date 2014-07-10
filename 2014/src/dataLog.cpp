#include "WPILib.h"
#include "stdio.h"
#include "dataLog.hpp"
#include <time.h>
#include <string>
#include <sstream>

std::string DataLog::currDateTime()
{
    time_t t;
    struct tm * timeInfo;
    char buffer[80];

    time(&t);
    timeInfo = localtime(&t);
    strftime(buffer, 80, "%c", timeInfo);
    return buffer;
}

DataLog::DataLog(std::string filename_)
{
    filename = filename_ + "_LOG.txt";

    // create the file or move and erase the last file
    FILE *tmp;
    tmp = fopen(filename.c_str(), "r");
    if (tmp != NULL)
    {
        std::string filePath = "logs/" + currDateTime() + filename;
        rename(filename.c_str(), filePath.c_str());
    }
    fclose(tmp);

    FILE *file;
    file = fopen(filename.c_str(), "w");
    fclose(file);
}

template<typename T>
std::string DataLog::asString(T in)
{
    std::stringstream ss;
    ss << in;
    return ss.str();
}

std::string DataLog::currSystemTime()
{
    time_t t;
    t = time(NULL);
    float min = t/60;
    float hours = min/60;
    float sec = t - hours - min;

    return asString(hours) + ":" + asString(min) + ":" + asString(sec) + ", ";
}

void DataLog::log(std::string data, bool attachTime)
{
    if (attachTime)
        data = currSystemTime() + data + "\n";
    else
        data = data + "\n";

    FILE *file;
    file = fopen(filename.c_str(), "a");
    fputs(data.c_str(), file);
    fclose(file);
}
