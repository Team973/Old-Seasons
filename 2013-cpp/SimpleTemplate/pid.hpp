#include "WPILib.h"

class PID
{
private:
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
public:
    PID(double p_=0.0, double i_=0.0, double d_=0.0);
    void setTarget(float target_);
    void reset();
    void start();
    void stop();
    void setICap(float icap_);
    void setBounds(float min_, float max_);
    float update(float actual);
};
