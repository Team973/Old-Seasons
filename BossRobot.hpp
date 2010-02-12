//
//	BossRobot.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/11/10.
//

#include "Options.hpp"
#include "WPILib.h"

#ifdef FEATURE_CAMERA
#	include "Vision/AxisCamera.h"
#endif

#ifndef _BOSS_973_BOSSROBOT_H_
#define _BOSS_973_BOSSROBOT_H_

class State;
class ControlBoard;
class DriveSystem;

class BossRobot : public SimpleRobot
{
protected:
	// New variables
	State *m_state, *m_prevState;
	ControlBoard *m_controlBoard;
	DriveSystem *m_driveSystem;
	
	// Old variables
	SpeedController *m_leftMotor1, *m_leftMotor2;   // Front, Back
	SpeedController *m_rightMotor1, *m_rightMotor2; // Front, Back
	Relay *m_compressor;
	DigitalInput *m_pressureSwitch;
	Solenoid *m_gearSwitch;
#ifdef FEATURE_PRECISION_DRIVE
	Encoder *m_leftEncoder, *m_rightEncoder;
#endif
#ifdef FEATURE_CAMERA
	AxisCamera *m_camera;
#endif
	
	Timer *m_ioTimer, *m_visionTimer;
	
	bool m_hiGear, m_still, m_prevStill;
public:
	BossRobot();
	
	virtual void Autonomous();
	virtual void OperatorControl();
	
	inline State *GetState() 				{ return m_state; }
	inline ControlBoard *GetControlBoard() 	{ return m_controlBoard; }
	inline DriveSystem *GetDriveSystem() 	{ return m_driveSystem; }
	RobotDrive *GetDrive();

protected:
	void SendIOPortData(void);
	void SendVisionData(void);
};

#endif
