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
    int discsFired;
    int targetFlywheelSpeed;
    DigitalInput pressureSwitch;

    public:
    OffseasonRobot(void):
        // Initializing objects
        //myRobot(1, 2),	// these must be initialized in the same order
        stick1(1),		// as they are declared above.
        stick2(2),
        comp(14,8),
        myShooter(new Shooter),
        myDrive(new Drive),
        hanger(5),
        pressureSwitch(14)
    {
        //myRobot.SetExpiration(0.1);
        //Starting compressor and banner sensor
        comp.Start();
        hanging = false;
        SmartDashboard::init();
    }


    void Autonomous(void)
    {
        autoTimer.Start();
        autoTimer.Reset();
        // Insert awesome autonomous here
        while (IsAutonomous())
        {
            myShooter->setFlywheelRunning(true);
            if (autoTimer.Get() >= 1.5)
            {
                if (autoTimer.Get() >= 2)
                {
                    myShooter->setIndexer(true);
                }
                if (autoTimer.Get() >= 3.5)
                {
                    myShooter->setIndexer(false);
                }
                if (autoTimer.Get() >= 4)
                {
                    myShooter->setIndexer(true);
                }
                if (autoTimer.Get() >= 5.5)
                {
                    myShooter->setIndexer(false);
                }
                if (autoTimer.Get() >= 6)
                {
                    myShooter->setIndexer(true);
                }
                if (autoTimer.Get() >= 7.5)
                {
                    myShooter->setIndexer(false);
                }
                if (autoTimer.Get() >= 8)
                {
                    myShooter->setIndexer(true);
                }
                if (autoTimer.Get() >= 9.5)
                {
                    myShooter->setIndexer(false);
                }
                if (autoTimer.Get() >= 10)
                {
                    myShooter->setIndexer(true);
                }
                if (autoTimer.Get() >= 11.5)
                {
                    myShooter->setIndexer(false);
                }
                if (autoTimer.Get() >= 12)
                {
                    myShooter->setIndexer(true);
                }
                if (autoTimer.Get() >= 13.5)
                {
                    myShooter->setIndexer(false);
                }
            }

            //myShooter->setShotAngle(false);
            myShooter->update();
            myDrive->update(0, 0, false, false);
        }
        myShooter->setFlywheelRunning(false);
        myShooter->setIndexer(false);
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

            // Dashboard updates
            myShooter->dashboardUpdate();
            SmartDashboard::PutBoolean("PRESSURE", pressureSwitch.Get());

            // The controls

            // Joystick 1
            myDrive->setKickUp(stick1.GetRawButton(7));
            myDrive->setBackWheelsDown(stick1.GetRawButton(8));
            myDrive->update(stick1.GetRawAxis(3), stick1.GetY(), stick1.GetRawButton(6), stick1.GetRawButton(5));
            if (stick1.GetRawButton(1))
                setHanging(false);
            if (stick1.GetRawButton(3))
                setHanging(true);

            // Joystick 2
            if (stick2.GetRawButton(1))
                myShooter->setShotAngle(false);
            if ((stick2.GetRawButton(2)) || (stick2.GetRawButton(5)))
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

