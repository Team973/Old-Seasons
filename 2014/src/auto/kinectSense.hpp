#include "autoCommand.hpp"

#ifndef KINECT_SENSE_H
#define KINECT_SENSE_H

#define HOT_ONE_BALL_CENTER 1
#define HOT_ONE_BALL_SIDE 2
#define HOT_TWO_BALL_CENTER 3
#define HOT_ONE_BALL_LANES 4
#define HOT_TWO_BALL_LANES 5

class KinectHandler;
class Drive;
class SequentialCommand;

class KinectSense : public AutoCommand
{
public:
    KinectSense::KinectSense(KinectHandler *kinect_, Drive *drive_, int autoMode_, float timeout_, std::string side_="none", std::string lane_="none", bool doubleTime_=false);
    virtual void Init();
    virtual bool Run();
private:
    void clear();


    KinectHandler *kinect;
    Drive *drive;
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
    std::string lane;
    bool doubleTime;
};

#endif
