#ifndef AUTO_MANAGER_HPP
#define AUTO_MANAGER_HPP

#include <map>
#include "auto/autoSequencer.hpp"
#include "auto/waitCommand.hpp"

class Wait;

class AutoManager
{
public:
    AutoManager();
private:
    std::map<std::string, AutoSequencer*> modes;

    Wait* waitCommand;
};

#endif
