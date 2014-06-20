#include "WPILib.h"
#include "stdio.h"
#include "dataLog.hpp"

DataLog::DataLog(std::string filename_)
{
    filename = filename_ + ".txt";

    // create the file or move and erase the last file
    if (fopen(filename.c_str(), "r") != NULL)
    {
        std::string filePath = "../logs/" + filename;
        rename(filename.c_str(), filePath.c_str());
    }

    FILE *file;
    file = fopen(filename.c_str(), "w");
    fclose(file);
}

void DataLog::log(std::string data)
{
    data = data + "\n";
    FILE *file;
    file = fopen(filename.c_str(), "a");
    fputs(data.c_str(), file);
    fclose(file);
}
