#include "constants.hpp"

Constants* Constants::instance = NULL;

void Constants::Extend()
{
    printf("extending constants file\n");
    if (instance == NULL)
    {
        instance = new Constants();
    }
}

Constants::Constants()
{
    addConstant("leftFrontDriveMotor", 0);

    printf("preparing to read constants from file\n");
    readConstantsFile();
}

ConstantsBase::Constant* Constants::getConstant(std::string name)
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
    constants.push_back(new Constant(name, value));
}
