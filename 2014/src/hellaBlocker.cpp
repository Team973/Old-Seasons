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
    if (position == "front")
    {
        reset();
    }
    extender->Set(true);
    position = "back";
}

void HellaBlocker::front()
{
    extender->Set(false);
    rotationTimer->Start();

    if (rotationTimer->Get() >= 2.5)
    {
        extender->Set(true);
        position = "front";
    }

    rotator->Set(true);
}

void HellaBlocker::reset()
{
    extender->Set(false);
    rotationTimer->Start();

    if (rotationTimer->Get() >= 2.5)
    {
        extender->Set(true);
        position = "back";
    }

    rotator->Set(false);
}
