//
//	DriveSystem.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/11/10.
//

#include "WPILib.h"
#include "BossRobot.hpp"
#include "SimplePID.hpp"

#ifndef _BOSS_973_DRIVESYSTEM_H_
#define _BOSS_973_DRIVESYSTEM_H_

class DriveSystem
{
protected:
	BossRobot *m_robot;
	RobotDrive *m_drive;
	
	float m_leftSpeed, m_rightSpeed;
	bool m_prevMoving;
	SimplePID m_leftPID, m_rightPID;
	
	DriveSystem();
	DriveSystem(BossRobot *, RobotDrive *);
public:
	virtual ~DriveSystem() {}
	
	inline RobotDrive *GetDrive()
	{
		return m_drive;
	}
	
	virtual void ReadControls() = 0;
	virtual void Drive();
	// The compensation needs to be done before calling Drive.
	virtual void Compensate();
	
	virtual void Stop();
	virtual bool IsMoving();
protected:
	virtual void InertCompensate();
	virtual void MovingCompensate();
};

/**
 * 	This class is actually useful for modes outside of autonomous.
 * 	The only thing this does is just allows driving programmatically, so you can
 *  use this if you don't want the operator to be able to drive for a while.
 */
class AutonomousDriveSystem : public DriveSystem
{
public:
	AutonomousDriveSystem(BossRobot *, RobotDrive *);
	virtual void ReadControls();
};

class TeleoperatedDriveSystem : public DriveSystem
{
protected:
	
	TeleoperatedDriveSystem() : DriveSystem() {}
	TeleoperatedDriveSystem(BossRobot *r, RobotDrive *d) : DriveSystem(r, d) {}
public:
	virtual bool IsMoving() = 0;
};

class ArcadeDriveSystem : public TeleoperatedDriveSystem
{
protected:
	float m_move, m_rotate;
	
	virtual void InterpretControls();
public:
	ArcadeDriveSystem(BossRobot *, RobotDrive *);
	virtual void ReadControls();
	virtual bool IsMoving();
};

class XboxDriveSystem : public ArcadeDriveSystem
{
public:
	XboxDriveSystem(BossRobot *, RobotDrive *);
	virtual void ReadControls();
};

#endif
