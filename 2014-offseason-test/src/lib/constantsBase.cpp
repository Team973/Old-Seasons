#include "constantsBase.hpp"
#include "../utility.hpp"
#include "../txtParser.hpp"
#include <stdlib.h>

ConstantsBase::ConstantsBase()
{
}

void ConstantsBase::readConstantsFile()
{
    TxtParser* file = new TxtParser("constants.txt");
    std::vector<std::string> lines = file->getContent();

    if (lines.size() < 1)
    {
        printf("the constants file is empty");
        return;
    }

    for (unsigned int n=0;n<lines.size();n++)
    {
        std::vector<std::string> line = file->split(file->scrape(lines[n], ' '), '=', n);

        if (line[0] == line.back())
        {
            printf("the item %s does not have a value\n", line[0].c_str());
        }
        else
        {
            for (unsigned int k=0;k<line.size()-1;k++)
            {
                for (unsigned int v=0;v<constants.size();v++)
                {
                    if (asLower(line[k].c_str()) == constants[v]->getName().c_str())
                    {
                        constants[v]->setValue(atof(line.back().c_str()));
                        printf("adding constant %s with value of %s\n", line[k].c_str(), line.back().c_str());
                    }
                }
            }
        }


    }
}
