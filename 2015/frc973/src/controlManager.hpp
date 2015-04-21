#ifndef CONTROLMANAGER_HPP_
#define CONTROLMANAGER_HPP_

namespace frc973 {

class ControlMap;
class StateManager;
class GrabManager;

class ControlManager {
public:
    ControlManager(ControlMap *controls_, StateManager *stateManager_, GrabManager *grabManager_);
    void update();
private:

    ControlMap *controls;
    StateManager *stateManager;
    GrabManager *grabManager;

    bool clawClosedOverriden;
    bool initialRetract;
};

} /* namespace frc973 */

#endif /* CONTROLMANAGER_HPP_ */
