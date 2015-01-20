#ifndef SUBSYSTEM_LISTER_HPP
#define SUBSYSTEM_LISTER_HPP

#include <string>
#include <map>

namespace frc973 {

class SubsystemBase;

class SubsystemLister
{
public:
    SubsystemLister();
    void addSubsystem(std::string name, SubsystemBase *s);
    SubsystemBase* addReturnSubsystem(std::string name, SubsystemBase *s);
    static SubsystemLister* getInstance();
    bool isSubsystem(std::string name);
    SubsystemBase* getSubsystem(std::string name);
private:
    static SubsystemLister *singleInstance;
    std::map<std::string, SubsystemBase*> subsystems;
};

}

#endif
