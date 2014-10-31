#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "lib/constantsBase.hpp"

class Constants : public ConstantsBase
{
public:
    Constants();
    static void Extend();

    static Constant *testConstant;
private:
    static Constants* instance;
};


#endif
