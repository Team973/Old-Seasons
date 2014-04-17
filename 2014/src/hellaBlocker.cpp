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

    if (rotationTimer->Get() >= 1)
    {
        extender->Set(true);
        rotationTimer->Stop();
        rotationTimer->Reset();
        position = "front";
    }
    if (rotationTimer->Get() >= 0.5)
    {
        rotator->Set(true);
    }
}

void HellaBlocker::reset()
{
    extender->Set(false);
    rotationTimer->Start();

    if (rotationTimer->Get() >= 1)
    {
        extender->Set(true);
        rotationTimer->Stop();
        rotationTimer->Reset();
        position = "back";
    }
    if (rotationTimer->Get() >= 0.5)
    {
        rotator->Set(false);
    }
}
