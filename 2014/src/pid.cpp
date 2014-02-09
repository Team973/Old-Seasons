#include "WPILib.h"
#include "pid.hpp"

PID::PID(double p_, double i_, double d_)
{
    p = p_;
    i = i_;
    d = d_;

    target = 0;
    min = 0;
    max = 0;

    integral = 0;
    iterm = 0;
    icap = 0;

    output = 0;
    prevErr = 0;

    timer = new Timer();

    reset();
}

void PID::setTarget(float target_)
{
    target = target_;
}

float PID::getTarget()
{
    return target;
}

void PID::reset()
{
    target = 0;
    integral = 0;
    output = 0;
    prevErr = 0;
    timer->Reset();
}

void PID::start()
{
    timer->Start();
    timer->Reset();
}

void PID::stop()
{
    timer->Stop();
}

void PID::setICap(float icap_)
{
    icap = icap_;
}

void PID::setBounds(float min_, float max_)
{
    min = min_;
    max = max_;
}

float PID::update(float actual)
{
    err = target - actual;

    // Calculate integral
    integral = integral + err * timer->Get();
    iterm = i * integral;
    if (icap)
    {
        if (iterm > icap)
        {
            iterm = icap;
        }
        else if (iterm < -icap)
        {
            iterm = -icap;
        }
    }

    // Calculate derivative
    derivative = (err - prevErr) / timer->Get();

    output = (p * err) + iterm + (d * derivative);
    prevErr = err;

    if (max && (output > max))
    {
        output = max;
    }
    if (min && (output < min))
    {
        output = min;
    }

    return output;
}
