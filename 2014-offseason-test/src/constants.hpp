#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "lib/constantsBase.hpp"

class Constants : public ConstantsBase
{
public:
    Constants();
    static void Extend();

    static ConstantsBase::Constant* getConstant(std::string name);
    void addConstant(std::string name, double value);

    ~Constants(){}
private:
    static Constants* instance;
};

#endif
