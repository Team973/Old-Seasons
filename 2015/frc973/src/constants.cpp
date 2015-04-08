#include "constants.hpp"
#include "lib/txtFileIO.hpp"
#include "lib/utility.hpp"
#include <stdio.h>

namespace frc973 {

std::vector<Constants::Constant*> Constants::constants;

void Constants::Extend()
{
    addConstant("kLinVelFF", 0.08);
    addConstant("kLinAccelFF", 0.03);
    addConstant("kAngVelFF", 0.0);
    addConstant("kAngAccelFF", 0.0);
    addConstant("kDriveP", 0.0);
    addConstant("kDriveI", 0.0);
    addConstant("kDriveD", 0.0);
    addConstant("kTurnP", 0.0);
    addConstant("kTurnI", 0.0);
    addConstant("kTurnD", 0.0);
    addConstant("kUpElevatorP", 0.5);
    addConstant("kUpElevatorI", 0.0);
    addConstant("kUpElevatorD", 0.0);
    addConstant("kDownElevatorP", 0.1);
    addConstant("kDownElevatorI", 0.0);
    addConstant("kDownElevatorD", 0.0);
    addConstant("kArmP", 0.0);
    addConstant("kArmI", 0.0);
    addConstant("kArmD", 0.0);
    addConstant("kArmVelFF", 0.0);
    addConstant("kGrabberDropTarget", 0.0);
    addConstant("kGrabberSettleTarget", 0.0);
    addConstant("kGrabberPullTarget", 0.0);
    addConstant("kGrabberP", 0.0);
    addConstant("kGrabberFaultTime", 0.0);
    addConstant("kGrabberFaultAngle", 0.0);
    addConstant("kGrabberDropTransAngle", 0.0);

    readConstantsFile();
}

Constants::Constant* Constants::getConstant(std::string name)
{
    for (unsigned int n=0;n<constants.size();n++)
    {
        if (constants[n]->getName().c_str() == asLower(name.c_str()))
        {
            return constants[n];
        }
    }
    printf("constant named %s was not found\n", name.c_str());
    return NULL;
}

void Constants::addConstant(std::string name, double value)
{
    constants.push_back(new Constants::Constant(name, value));
	SocketClient::UpdateDash(name, value);
}

void Constants::readConstantsFile()
{
    //printf("reading constants file\n");
    TxtReader* file = new TxtReader("/home/lvuser/constants.txt");
    std::vector<std::string> lines = file->getContent();

    if (lines.size() < 1)
    {
        printf("the constants file is empty\n");
        return;
    }


    for (unsigned int n=0;n<lines.size();n++)
    {
        std::vector<std::string> line = split(scrape(lines[n], ' '), '=', n);
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
                        break;
                    }
                }
            }
        }
    }
}

}
