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

std::string TxtParser::scrape(std::string line, char token)
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

std::vector<std::string> TxtParser::split(std::string line, char token, int lineNumber)
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

}
