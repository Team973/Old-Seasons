#include "constants.hpp"

Constants* Constants::instance = NULL;

ConstantsBase::Constant* Constants::testConstant = new ConstantsBase::Constant("testConstant", 15);

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
    printf("preparing to read constants from file\n");
    readConstantsFile();
}
