/**
 *	@file BossRobot.hpp
 *	Header for the BossRobot class.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 2/11/10.
 */

#include "Options.hpp"
#include "WPILib.h"
#include "MyDriverStationLCD.h"
#include "ConfigParser.hpp"

#ifdef FEATURE_CAMERA
#	include "Vision/AxisCamera.h"
#endif

#ifndef _BOSS_973_BOSSROBOT_H_
#define _BOSS_973_BOSSROBOT_H_

class State;
class DriveSystem;
class KickerSystem;

/**
 *	Main controller for the robot.
 *
 *	Most of what is contained in here is the basic code to get the robot
 *  initialized and running, and then the core logic is handled by State
 *	subclasses.
 */
class BossRobot : public SimpleRobot
{
private:
	State *m_state, *m_prevState;
	DriveSystem *m_driveSystem;
	KickerSystem *m_kickerSystem;
	ConfigParser m_config;
	
	SpeedController *m_leftMotor1, *m_leftMotor2;   // Front, Back
	SpeedController *m_rightMotor1, *m_rightMotor2; // Front, Back
	Solenoid *m_gearSwitch;
	Encoder *m_leftDriveEncoder, *m_rightDriveEncoder;
	AnalogChannel *m_gyroChannel;
	Gyro *m_gyro;
	
	Relay *m_compressor;
	DigitalInput *m_pressureSwitch;
	
	SpeedController *m_shoulderMotor1, *m_shoulderMotor2;
	AnalogChannel *m_shoulderSensor;
	
	SpeedController *m_kickerMotor;
	Relay *m_kickerWinchRelay1, *m_kickerWinchRelay2;
	AnalogChannel *m_kickerWinchSensor;
	AnalogChannel *m_kickerEncoder;
	SpeedController *m_intakeMotor;
	
#ifdef FEATURE_CAMERA
	AxisCamera *m_camera;
#endif
	
	Timer *m_ioTimer, *m_visionTimer;
public:
	/** Initialize the robot */
	BossRobot();
	
	/**
	 *	Handle autonomous mode.
	 *
	 *	This method will be called when autonomous is started, and it is the
	 *	responsibility of this method to exit when autonomous is over.
	 */
	virtual void Autonomous();
	
	/**
	 *	Handle teleoperated mode.
	 *
	 *	This method will be called when teleoperated is started, and it is the
	 *	responsibility of this method to exit when teleoperated is over.
	 *
	 *	When this method is called, it will switch the current state to be an
	 *	instance of NormalState.
	 */
	virtual void OperatorControl();
	
	/** Get the current state. */
	inline State *GetState() 				{ return m_state; }
	
	/**
	 *	Switch the current state.
	 *
	 *	The new state will not be changed to immediately.  When the run loop
	 *	hits the next iteration, it will discover that the state has changed,
	 *	call State::Exit on the old state and State::Enter on the new state.
	 */
	void ChangeState(State *);
	
	inline DriveSystem *GetDriveSystem() 	{ return m_driveSystem; }
	void SetDriveSystem(DriveSystem *d);
	
	inline KickerSystem *GetKickerSystem() 	{ return m_kickerSystem; }
	
	/**
	 *	Get the robot's configuration.
	 *
	 *	The values inside the configuration parser are already initialized with
	 *	the contents of the "boss.cfg" file.
	 */
	inline ConfigParser &GetConfig()		{ return m_config; }
	
	// Accessors
	inline SpeedController *GetLeftFrontDriveMotor()	{ return m_leftMotor1; }
	inline SpeedController *GetLeftRearDriveMotor()		{ return m_leftMotor2; }
	inline SpeedController *GetRightFrontDriveMotor()	{ return m_rightMotor1; }
	inline SpeedController *GetRightRearDriveMotor()	{ return m_rightMotor2; }
	
	inline Encoder *GetLeftDriveEncoder() 	{ return m_leftDriveEncoder; }
	inline Encoder *GetRightDriveEncoder() 	{ return m_rightDriveEncoder; }
	inline Gyro *GetGyro() 				{ return m_gyro; }
	inline Relay *GetKickerWinch1()			{ return m_kickerWinchRelay1; }
	inline Relay *GetKickerWinch2()			{ return m_kickerWinchRelay2; }
	inline AnalogChannel *GetKickerWinchSensor()	{ return m_kickerWinchSensor; }
	inline AnalogChannel *GetKickerEncoder()	{ return m_kickerEncoder; }
	inline SpeedController *GetKickerMotor()	{ return m_kickerMotor; }
	inline Solenoid *GetGearSwitch()		{ return m_gearSwitch; }
	inline SpeedController *GetIntakeMotor()	{ return m_intakeMotor; }
	
	inline SpeedController *GetShoulderMotor1()	{ return m_shoulderMotor1; }
	inline SpeedController *GetShoulderMotor2()	{ return m_shoulderMotor2; }
	inline AnalogChannel *GetShoulderSensor()	{ return m_shoulderSensor; }

protected:
	void SendIOPortData(void);
	void SendVisionData(void);
	
	void PreStep(void);
	void PostStep(void);
};

#endif
