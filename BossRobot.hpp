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
class DriveSystem;

class BossRobot : public SimpleRobot
{
protected:
	State *m_state, *m_prevState;
	DriveSystem *m_driveSystem;
	
	SpeedController *m_leftMotor1, *m_leftMotor2;   // Front, Back
	SpeedController *m_rightMotor1, *m_rightMotor2; // Front, Back
	Solenoid *m_gearSwitch;
	Encoder *m_leftDriveEncoder, *m_rightDriveEncoder;
	
	Relay *m_compressor;
	DigitalInput *m_pressureSwitch;
	
	SpeedController *m_armMotor1, *m_armMotor2;
	SpeedController *m_intakeMotor1, *m_intakeMotor2;
	SpeedController *m_kickerMotor;
	Relay *m_winchRelay1, *m_winchRelay2;
	Relay *m_intakeRelay1, *m_intakeRelay2;
	
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
	
	// Accessors
	inline Encoder *GetLeftDriveEncoder() 	{ return m_leftDriveEncoder; }
	inline Encoder *GetRightDriveEncoder() 	{ return m_rightDriveEncoder; }

protected:
	void SendIOPortData(void);
	void SendVisionData(void);
};

#endif
