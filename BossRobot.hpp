//
//	BossRobot.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/11/10.
//

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

class BossRobot : public SimpleRobot
{
protected:
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
	
	SpeedController *m_armMotor1, *m_armMotor2;
	SpeedController *m_intakeMotor1, *m_intakeMotor2;
	Relay *m_intakeMotor3, *m_intakeMotor4;
	
	SpeedController *m_kickerMotor;
	Relay *m_kickerWinchRelay1, *m_kickerWinchRelay2;
	AnalogChannel *m_kickerWinchSensor;
	AnalogChannel *m_kickerEncoder;
	
#ifdef FEATURE_CAMERA
	AxisCamera *m_camera;
#endif
	
	Timer *m_ioTimer, *m_visionTimer;
public:
	BossRobot();
	
	virtual void Autonomous();
	virtual void OperatorControl();
	
	inline State *GetState() 				{ return m_state; }
	void ChangeState(State *);
	
	inline DriveSystem *GetDriveSystem() 	{ return m_driveSystem; }
	void SetDriveSystem(DriveSystem *d);
	RobotDrive *GetDrive();
	
	inline KickerSystem *GetKickerSystem() 	{ return m_kickerSystem; }
	
	inline ConfigParser &GetConfig()		{ return m_config; }
	
	// Accessors
	inline Encoder *GetLeftDriveEncoder() 	{ return m_leftDriveEncoder; }
	inline Encoder *GetRightDriveEncoder() 	{ return m_rightDriveEncoder; }
	inline Gyro *GetGyro() 				{ return m_gyro; }
	inline Relay *GetKickerWinch1()			{ return m_kickerWinchRelay1; }
	inline Relay *GetKickerWinch2()			{ return m_kickerWinchRelay2; }
	inline AnalogChannel *GetKickerWinchSensor()	{ return m_kickerWinchSensor; }
	inline AnalogChannel *GetKickerEncoder()	{ return m_kickerEncoder; }
	inline SpeedController *GetKickerMotor()	{ return m_kickerMotor; }
	inline Solenoid *GetGearSwitch()		{ return m_gearSwitch; }

protected:
	void SendIOPortData(void);
	void SendVisionData(void);
	
	void PreStep(void);
	void PostStep(void);
};

#endif
