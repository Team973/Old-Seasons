#ifndef CONTROLMANAGER_HPP_
#define CONTROLMANAGER_HPP_

namespace frc973 {

class ControlMap;
class StateManager;

class ControlManager {
public:
    ControlManager(ControlMap *controls_, StateManager *stateManager_);
    void update();
private:

    ControlMap *controls;
    StateManager *stateManager;
};

} /* namespace frc973 */

#endif /* CONTROLMANAGER_HPP_ */
