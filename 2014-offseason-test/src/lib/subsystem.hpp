#ifndef SUBSYSTEM_HPP
#define SUBSYSTEM_HPP

class Subsystem
{
public:
    Subsystem();
    virtual void setBehavior();
    virtual void update();

    virtual ~Subsystem() {};
};

#endif 
