#ifndef AUTO_MANAGER_HPP
#define AUTO_MANAGER_HPP

#include <map>
#include "auto/autoSequencer.hpp"
#include "lib/socketClient.hpp"

namespace frc973 {

class Wait;
class StateManager;
class GrabManager;

class AutoManager : public SocketClient::SocketListener
{
public:
    AutoManager(StateManager *stateManager_, GrabManager *grabManager_);
    void resetModes();
    void setMode(std::string mode);
    void setModes();
    void nextMode();
    AutoSequencer* getCurrentMode();
    std::string getCurrentName();
    void OnValueChange(std::string varName, std::string newValue);
private:

    StateManager *stateManager;
    GrabManager *grabManager;

    std::map<std::string, AutoSequencer*> modes;

    std::map<std::string, AutoSequencer*>::iterator it;
};

}

#endif
