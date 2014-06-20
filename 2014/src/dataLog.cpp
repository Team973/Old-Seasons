#include "WPILib.h"
#include "stdio.h"
#include "dataLog.hpp"
#include <time.h>
#include <string>

DataLog::DataLog(std::string filename_)
{
    filename = filename_ + ".txt";

    // create the file or move and erase the last file
    FILE *tmp;
    tmp = fopen(filename.c_str(), "r");
    if (tmp != NULL)
    {
        std::string filePath = "logs/" + filename;
        rename(filename.c_str(), filePath.c_str());
    }
    fclose(tmp);

    FILE *file;
    file = fopen(filename.c_str(), "w");
    fclose(file);
}

void DataLog::currTime()
{
    time_t t;
    t = time(NULL);
    float min = t/60;
    float hours = min/60;
    float sec = t - hours - min;

    timeStamp = std::to_string(hours) + ":" + std::to_string(min) + ":" + std::to_string(sec) + " ";
}

void DataLog::log(std::string data)
{
    data =timeStamp + data + "\n";
    FILE *file;
    file = fopen(filename.c_str(), "a");
    fputs(data.c_str(), file);
    fclose(file);
}
