#ifndef PID_HPP
#define PID_HPP

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
    float err;
    float derivative;

    float pterm;
    float dterm;

    Timer* timer;
public:
    PID(double p_=0.0, double i_=0.0, double d_=0.0);
    PID(double gains[]);
    void setGains(double p_=0.0, double i_=0.0, double d_=0.0);
    void setGains(double gains[]);
    void setTarget(float target_);
    float getTarget();
    void reset();
    void start();
    void stop();
    void setICap(float icap_);
    void setBounds(float min_, float max_);
    float update(float actual);
    float update(float actual, Timer *t);
    float getP();
    float getI();
    float getD();
};

#endif
