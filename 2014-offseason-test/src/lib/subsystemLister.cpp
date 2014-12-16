#include "subsystemLister.hpp"
#include "subsystemBase.hpp"
#include "../utility.hpp"

namespace frc973 {

SubsystemLister* SubsystemLister::singleInstance = NULL;

SubsystemLister* SubsystemLister::getInstance()
{
    if (singleInstance == NULL)
    {
        singleInstance = new SubsystemLister();
    }
    return singleInstance;
}

SubsystemLister::SubsystemLister()
{
}

void SubsystemLister::addSubsystem(std::string name, SubsystemBase *s)
{
    subsystems[asLower(name)] = s;
}

SubsystemBase* SubsystemLister::addReturnSubsystem(std::string name, SubsystemBase *s)
{
    subsystems[asLower(name)] = s;
    return s;
}

bool SubsystemLister::isSubsystem(std::string name)
{
    if (subsystems[asLower(name)] != NULL)
        return true;

    return false;
}

SubsystemBase* SubsystemLister::getSubsystem(std::string name)
{
    if (isSubsystem(name))
    {
        return subsystems[asLower(name)];
    }

    return NULL;
}

}
