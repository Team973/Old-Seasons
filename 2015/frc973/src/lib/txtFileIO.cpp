#include "WPILib.h"
#include "txtFileIO.hpp"
#include "utility.hpp"
#include <stdio.h>
#include <vector>

namespace frc973 {

Channel<std::string> TxtWriter::msgChan(3);
Channel<std::string> TxtWriter::pathChan(3);

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

void TxtWriter::Write(std::string filePath, std::string msg)
{
    pathChan.send(filePath);
    msgChan.send(msg);
}

void TxtWriter::Run()
{
    while (true)
    {
        std::string lines;

        std::string filePath = pathChan.recv();
        FILE *pFile = fopen(filePath.c_str(), "w+");

        if (pFile != NULL)
        {
            lines = msgChan.recv();
            fputs(lines.c_str(), pFile);
        }
        else
        {
            printf("The file %s does not exist\n", filePath.c_str());
        }

        fclose(pFile);
    }
}

}
