#ifndef GREY_JOY_H
#define GREY_JOY_H

class GreyJoy
{
public:
    GreyJoy(int port);
    bool getButton(std::string key);
    bool getToggle(std::string key);
    float getAxis(std::string key);
    void update();
private:
    Joystick *joy;

    struct stick {
        std::string name;
        float oldValue;
        float newValue;
        int number;
    };

    struct button {
        std::string name;
        bool oldValue;
        bool newValue; 
        bool toggle;
        Timer spamTimer;
        int number;
    };

    std::vector<button> buttons;
    std::vector<stick> sticks;
};

#endif
