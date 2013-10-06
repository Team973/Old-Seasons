#include "WPILib.h"
#include "shooter.hpp"

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class OffseasonRobot : public SimpleRobot
{
    //RobotDrive myRobot; // robot drive system
    Joystick stick; // only joystick
    
    // declaring objects
    Compressor comp;
    Shooter *myShooter;

    public:
    OffseasonRobot(void):
        // Initializing objects
        //myRobot(1, 2),	// these must be initialized in the same order
        stick(1),		// as they are declared above.
        comp(14,8)
    {
        //myRobot.SetExpiration(0.1);
        //Starting compressor and banner sensor
        comp.Start();
    }


    void Autonomous(void)
    {
    // Insert awesome autonomous here
    }


    // The following computes and controls the speed of the flywheel
    // TODO: (oliver) Refactor functions into seperate class

    void OperatorControl(void)
    {

        //myRobot.SetSafetyEnabled(true);
        while (IsOperatorControl())
        {
            //myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
            
            // The controls
            //indexer.Set(stick.GetRawButton(1));
            //shotAngle.Set(stick.GetRawButton(2));
            //roller.Set(stick.GetRawButton(5));
            /*
            if (stick.GetRawButton(8) == true)
                flywheelMotor.Set(RPMcontrol(getFlywheelSpeed()));
            else
                flywheelMotor.Set(0);
                */

            Wait(0.005);				// wait for a motor update time
        }
    }

    /**
     * Runs during test mode
     */
    void Test() {

    }
};

START_ROBOT_CLASS(OffseasonRobot);

