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
    bool hanging;
    Solenoid hanger;
    Timer autoTimer;

    public:
    OffseasonRobot(void):
        // Initializing objects
        //myRobot(1, 2),	// these must be initialized in the same order
        stick1(1),		// as they are declared above.
        stick2(2),
        comp(14,8),
        myShooter(new Shooter),
        myDrive(new Drive),
        hanger(5)
    {
        //myRobot.SetExpiration(0.1);
        //Starting compressor and banner sensor
        comp.Start();
        hanging = false;
    }


    void Autonomous(void)
    {
        // Insert awesome autonomous here
        autoTimer.Start();
        myShooter->setShotAngle(true);
        while (IsAutonomous())
        {
            if (autoTimer.Get() >= 4)
            {
                myShooter->autoFire();
            }

            myShooter->update();
            myDrive->update(0, 0, false, false);
        }
    }

    void setHanging(bool h)
    {
        hanging = h;
    }

    void OperatorControl(void)
    {

        //myRobot.SetSafetyEnabled(true);
        while (IsOperatorControl())
        {
            // Updates:

            // Shooter
            myShooter->update();

            // Hanger
            hanger.Set(hanging);

            // The controls

            // Joystick 1
            myDrive->update(stick1.GetRawAxis(3), stick1.GetY(), stick1.GetRawButton(6), stick1.GetRawButton(5));
            if (stick1.GetRawButton(1))
                setHanging(false);
            if (stick1.GetRawButton(3))
                setHanging(true);

            // Joystick 2
            if (stick2.GetRawButton(1))
                myShooter->setShotAngle(false);
            if (stick2.GetRawButton(2))
                myShooter->setShotAngle(true);
            myShooter->setRollerRunning(stick2.GetRawButton(5));
            myShooter->setIndexer(stick2.GetRawButton(6));
            if (stick2.GetRawButton(8) == true)
                myShooter->setFlywheelRunning(true);
            else
                myShooter->setFlywheelRunning(false);
            // End Controls

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

