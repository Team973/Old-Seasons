#ifndef AUTO_MANAGER_HPP
#define AUTO_MANAGER_HPP

#include <vector>
#include "auto/autoSequencer.hpp"

namespace frc973 {

class Wait;
class Drive;

class AutoManager
{
public:
    AutoManager(Drive* drive_);
    void nextMode();
    AutoSequencer* getCurrentMode();
    std::string getCurrentName();
private:
    std::vector<AutoSequencer*> modes;

    unsigned int currMode;
};

}

#endif
