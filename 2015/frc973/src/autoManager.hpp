#ifndef AUTO_MANAGER_HPP
#define AUTO_MANAGER_HPP

#include <map>
#include "auto/autoSequencer.hpp"

namespace frc973 {

class Wait;
class StateManager;

class AutoManager
{
public:
    AutoManager(StateManager* stateManager_);
    void resetModes();
    void setModes();
    void nextMode();
    AutoSequencer* getCurrentMode();
    std::string getCurrentName();
private:

    StateManager *stateManager;

    std::map<std::string, AutoSequencer*> modes;

    std::map<std::string, AutoSequencer*>::iterator it;
};

}

#endif
