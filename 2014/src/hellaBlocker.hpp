#ifndef HELLA_BLOCKER_H
#define HELLA_BLOCKER_H

class HellaBlocker
{
public:
    HellaBlocker(Solenoid *extender_, Solenoid *rotator_);
    void back();
    void front();
private:
    Solenoid *extender;
    Solenoid *rotator;

    void reset();

    std::string position;

    Timer *rotationTimer;
};

#endif
