#include "WPILib.h"
#include "txtParser.hpp"
#include "utility.hpp"
#include <stdio.h>
#include <vector>

namespace frc973 {

TxtParser::TxtParser(std::string name)
{
    //printf("opening file\n");
    pFile = fopen(name.c_str(), "r");
    if (pFile != NULL)
    {
        //printf("file exists, getting contents\n");
        lines = getFile();
        printf("file read\n");
    }
    else
        printf("The file %s does not exist\n",name.c_str());
}

std::vector<std::string> TxtParser::getContent()
{
    return lines;
}

std::vector<std::string> TxtParser::getFile()
{
    std::vector<std::string> str;
    char lineBuffer[80];
    while (fgets(lineBuffer, 80, pFile) != NULL)
    {
        str.push_back(lineBuffer);
    }
    
    for (unsigned int n=0;n<str.size();n++)
    {
        if (str[n][0] == '#')
        {
            str.erase(str.begin()+n);
        }
    }
    return str;
}

}
