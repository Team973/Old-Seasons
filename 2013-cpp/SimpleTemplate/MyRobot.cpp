#include "WPILib.h"
#include "drive.hpp"
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
    Joystick stick1; // only joystick
    Joystick stick2;
    
    // declaring objects
    Compressor comp;
    Shooter *myShooter;
    Drive *myDrive;

    public:
    OffseasonRobot(void):
        // Initializing objects
        //myRobot(1, 2),	// these must be initialized in the same order
        stick1(1),		// as they are declared above.
        stick2(2),
        comp(14,8),
        myShooter(),
        myDrive()
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

    void OperatorControl(void)
    {

        //myRobot.SetSafetyEnabled(true);
        while (IsOperatorControl())
        {
            // The controls
            // Joystick 1
            myDrive->update(stick1.GetRawAxis(3), stick1.GetY(), stick1.GetRawButton(6), stick1.GetRawButton(5));

            // Joystick 2
            myShooter->setIndexer(stick2.GetRawButton(1));
            myShooter->setShotAngle(stick2.GetRawButton(2));
            myShooter->setRollerRunning(stick2.GetRawButton(5));
            if (stick2.GetRawButton(8) == true)
                myShooter->setFlywheelRunning(true);
            else
                myShooter->setFlywheelRunning(false);

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

