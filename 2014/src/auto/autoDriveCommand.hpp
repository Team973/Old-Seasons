#include "autoCommand.hpp"
#include "../pid.hpp"

#ifndef AUTODRIVECOMMAND_H
#define AUTODRIVECOMMAND_H

class Drive;

class AutoDriveCommand : public AutoCommand
{
private:
    float targetX;
    float targetY;
    bool backward;
    float drivePercision;
    float turnPercision;
    float driveCap;
    float turnCap;
    float arcCap;

    float theta;
    float magnitude;
    float deltaX;
    float deltaY;
    float prevTheta;
    float currTheta;
    float prevGyro;
    float currGyro;
    float prevLeft;
    float currLeft;
    float prevRight;
    float currRight;
    float prevX;
    float currX;
    float prevY;
    float currY;

    double PI;

    Drive *drive;

    Timer *driveTimer;

    PID* drivePID;
    PID* rotatePID;
    PID* anglePID;
public:
    AutoDriveCommand(Drive* drive_, float targetX_, float targetY_, bool backward_=false, double timeout_ = 0.0, float drivePercision_=6, float turnPercision_=5, float driveCap_=0.5, float turnCap_=0.7, float arcCap_=0.3);
    void resetDrive();
    void calculateDrive();
    void storeDriveCalculations();
    virtual void Init();
    virtual bool Run();
};

#endif
