#ifndef SUBSYSTEM_BASE_HPP
#define SUBSYSTEM_BASE_HPP

class SubsystemBase
{
public:
    SubsystemBase() {};
    virtual void setBehavior() {};
    virtual void update() {};

    virtual ~SubsystemBase() {};
};

#endif 
