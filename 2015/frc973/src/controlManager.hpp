#ifndef CONTROLMANAGER_HPP_
#define CONTROLMANAGER_HPP_

namespace frc973 {

class ControlMap;
class StateManager;
class GrabManager;

class ControlManager {
public:
    ControlManager(ControlMap *controls_, StateManager *stateManager_);
    void update();
private:

    ControlMap *controls;
    StateManager *stateManager;

    bool clawClosedOverriden;
    bool initialRetract;
    bool capButtonDelay;
};

} /* namespace frc973 */

#endif /* CONTROLMANAGER_HPP_ */
