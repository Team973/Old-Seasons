#ifndef SUBSYSTEM_BASE_HPP
#define SUBSYSTEM_BASE_HPP

namespace frc973 {

class SubsystemBase
{
public:
    SubsystemBase() {};
    virtual void setBehavior() {};
    virtual void update() {};

    virtual ~SubsystemBase() {};
};

}

#endif 
