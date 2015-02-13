#include "autoCommand.hpp"

#ifndef HELLAVATOR_H
#define HELLAVATOR_H

class Hellavator : public AutoCommand
{
public:
    Hellavator(Solenoid* hellavator_, float timeout);
    virtual void Init();
    virtual bool Run();
private:
    Solenoid *hellavator;
};

#endif
