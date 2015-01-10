#ifndef AUTO_MANAGER_HPP
#define AUTO_MANAGER_HPP

#include <map>
#include "auto/autoSequencer.hpp"

namespace frc973 {

class Wait;

class AutoManager
{
public:
    AutoManager();
    void nextMode();
    AutoSequencer* getCurrentMode();
    std::string getCurrentName();
private:
    std::map<std::string, AutoSequencer*> modes;

    std::map<std::string, AutoSequencer*>::iterator it;

};

}

#endif
