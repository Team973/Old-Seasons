#include "WPILib.h"
#include <vector>
#include <math.h>
#include "greyJoy.hpp"

GreyJoy::GreyJoy(int port)
{
    // since this object is a wrapper we can self-contain the joystick
    // and just pass the wrapper
    joy = new Joystick(port);

    //name all sticks
    sticks[1].name = "x";
    sticks[2].name = "y";
    sticks[3].name = "rx";
    sticks[4].name = "ry";
    sticks[5].name = "hatx";
    sticks[6].name = "haty";

    //name all buttons
    buttons[1].name = "x";
    buttons[2].name = "a";
    buttons[3].name = "b";
    buttons[4].name = "y";
    buttons[5].name = "lbumper";
    buttons[6].name = "rbumper";
    buttons[7].name = "ltrigger";
    buttons[8].name = "rtrigger";
    buttons[9].name = "select";
    buttons[10].name = "start";
    buttons[11].name = "lstick";
    buttons[12].name = "rstick";

    // inititalize all buttons
    for (int b=1;b<13;b++)
    {
        buttons[b].oldValue = buttons[b].newValue = buttons[b].toggle = false;
        buttons[b].spamTimer.Start();
    }
}

bool GreyJoy::getButton(std::string key, bool wantToggle)
{
    for (int n=1;n<13;n++)
    {
        if (buttons[n].name == key)
        {
            if (wantToggle)
            {
                return buttons[n].toggle;
            }
            else
            {
                return buttons[n].oldValue;
            }
        }
    }
    return false;
}

float GreyJoy::getAxis(std::string key)
{
    for (int n=1;n<7;n++)
    {
        if (sticks[n].name == key)
        {
            return sticks[n].oldValue;
        }
    }
    return 0.0;
}

void GreyJoy::update()
{
    // update the axi
    for (int a=1;a<7;a++)
    {
        sticks[a].newValue = joy->GetRawAxis(a);
        // this alleviates some of the touchyness that the drivers complained about
        if (fabs(sticks[a].oldValue - sticks[a].newValue) > .05)
        {
            sticks[a].oldValue = sticks[a].newValue;
        }
    }

    // update the buttons
    for (int b=1;b<13;b++)
    {
        if (buttons[b].newValue)
        {
            buttons[b].toggle = !buttons[b].oldValue;
        }
        buttons[b].oldValue = buttons[b].newValue;
    }
}
