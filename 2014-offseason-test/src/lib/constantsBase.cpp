#include "constantsBase.hpp"
#include "../utility.hpp"
#include "../txtParser.hpp"
#include <stdlib.h>

std::vector<ConstantsBase::Constant*> ConstantsBase::constants;

ConstantsBase::ConstantsBase()
{
}

void ConstantsBase::readConstantsFile()
{
    //printf("reading constants file\n");
    TxtParser* file = new TxtParser("constants.txt");
    std::vector<std::string> lines = file->getContent();

    if (lines.size() < 1)
    {
        printf("the constants file is empty\n");
        return;
    }


    for (unsigned int n=0;n<lines.size();n++)
    {
        std::vector<std::string> line = file->split(file->scrape(lines[n], ' '), '=', n);
        //printf("line has been split and scraped\n");

        if (line[0] == line.back())
        {
            printf("the item %s does not have a value\n", line[0].c_str());
        }
        else
        {
            //printf("all items in line have a value\n");
            for (unsigned int k=0;k<line.size()-1;k++)
            {
                //printf("the first for loop is entered NOW\n");
                for (unsigned int v=0;v<constants.size();v++)
                {
                    //printf("the second for loop is entered NOW\n");
                    if (asLower(line[k].c_str()) == constants[v]->getName().c_str())
                    {
                        //printf("constant found\n");
                        constants[v]->setValue(atof(line.back().c_str()));
                        printf("adding constant %s with value of %f\n", line[k].c_str(), atof(line.back().c_str()));
                        printf("constant %s has the value %f\n", constants[v]->getName().c_str(), constants[v]->getDouble());
                        break;
                    }
                }
            }
        }
    }
}
