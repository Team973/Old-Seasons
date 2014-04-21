#include "autoCommand.hpp"

#ifndef KINECT_SENSE_H
#define KINECT_SENSE_H

#define HOT_ONE_BALL_CENTER 1
#define HOT_ONE_BALL_SIDE 2
#define HOT_TWO_BALL_CENTER 3

class KinectHandler;
class Drive;
class Shooter;
class Arm;
class Intake;
class SequentialCommand;

class KinectSense : public AutoCommand
{
public:
    KinectSense(KinectHandler *kinect_, Drive *drive_, Shooter *shooter_, Arm *arm_, Intake *intake_, int autoMode_, std::string side_="none");
    virtual void Init();
    virtual bool Run();
private:
    void clear();


    KinectHandler *kinect;
    Drive *drive;
    Shooter *shooter;
    Arm *arm;
    Intake *intake;
    std::vector<AutoCommand*> sequence;
    SequentialCommand *cmd;

    float movement;
    int autoMode;
    bool init;
    bool movementSelected;
    bool finalSequence;
    bool left;
    bool right;

    // Turn Distances
    float jukeTurn;
    float centerTurn;

    std::string side;
};

#endif
