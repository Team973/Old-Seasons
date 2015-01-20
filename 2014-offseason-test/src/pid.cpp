#include "WPILib.h"
#include "pid.hpp"

namespace frc973 {

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
    derivative = 0;

    pterm = 0;
    dterm = 0;

    output = 0;
    prevErr = 0;

    timer = new Timer();

    reset();
}

PID::PID(double gains[])
{
    p = gains[0];
    i = gains[1];
    d = gains[2];

    target = 0;
    min = 0;
    max = 0;

    integral = 0;
    iterm = 0;
    icap = 0;
    derivative = 0;

    pterm = 0;
    dterm = 0;

    output = 0;
    prevErr = 0;

    timer = new Timer();

    reset();
}

void PID::setGains(double p_, double i_, double d_)
{
    p = p_;
    i = i_;
    d = d_;
}

void PID::setGains(double gains[])
{
    p = gains[0];
    i = gains[1];
    d = gains[2];
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
    if (icap != 0)
    {
        if (iterm > icap)
        {
            integral = icap/i;
        }
        else if (iterm < -icap)
        {
            integral = -icap/i;
        }
        iterm = i * integral;
    }

    // Calculate derivative
    derivative = (err - prevErr) / timer->Get();
    pterm = p * err;
    dterm = d * derivative;

    output = pterm + iterm + dterm;
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

float PID::update(float actual, Timer *t)
{
    err = target - actual;

    // Calculate integral
    integral = integral + err * t->Get();
    iterm = i * integral;
    if (icap != 0)
    {
        if (iterm > icap)
        {
            integral = icap/i;
        }
        else if (iterm < -icap)
        {
            integral = -icap/i;
        }
        iterm = i * integral;
    }

    // Calculate derivative
    derivative = (err - prevErr) / t->Get();

    pterm = p * err;

    dterm = d * derivative;

    output = pterm + iterm + dterm;
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

float PID::getP()
{
    return pterm;
}

float PID::getI()
{
    return iterm;
}

float PID::getD()
{
    return dterm;
}

}
