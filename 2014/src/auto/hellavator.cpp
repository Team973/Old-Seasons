#include "WPILib.h"
#include "hellavator.hpp"

Hellavator::Hellavator(Solenoid *hellavator_, float timeout_)
{
    hellavator = hellavator_;
    setTimeout(timeout_);
}

void Hellavator::Init()
{
    timer->Start();
    timer->Reset();

    hellavator->Set(true);
}

bool Hellavator::Run()
{
    if (timer->Get() >= timeout)
        return true;
    else
        return false;
}
