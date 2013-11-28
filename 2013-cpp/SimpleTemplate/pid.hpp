#include "WPILib.h"

class PID
protected:
{
    double p;
    double i;
    double d;
    
    float target;
    float min;
    float max;

    float integral;
    float icap;
    float iterm;

    float output;
    float prevErr;

    Timer timer;
}
public:
{
    void setTarget(float target_);
    void reset();
    void start();
    void stop();
    void setICap(float icap_);
    void setBounds(float min_, float max_);
    float update(float actual);
}
