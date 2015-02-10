#include "WPILib.h"
#include "hellaBlocker.hpp"

HellaBlocker::HellaBlocker(Solenoid *extender_, Solenoid *rotator_)
{
    extender = extender_;
    rotator = rotator_;

    rotationTimer = new Timer();

    position = "back";
}

void HellaBlocker::back()
{
    extender->Set(true);
}

void HellaBlocker::front()
{
    extender->Set(false);
    rotationTimer->Start();

    if (rotationTimer->Get() >= 1.5)
    {
        extender->Set(true);
    }

    rotator->Set(true);
}

void HellaBlocker::reset()
{
    rotationTimer->Stop();
    rotationTimer->Reset();
}
