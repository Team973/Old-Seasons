//
//	DriveSystem.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/11/10.
//

#include "WPILib.h"
#include "Options.hpp"
#include "BossRobot.hpp"
#include "SimplePID.hpp"

#ifndef _BOSS_973_DRIVESYSTEM_H_
#define _BOSS_973_DRIVESYSTEM_H_

class DriveSystem
{
protected:
	BossRobot *m_robot;
	
	float m_leftSpeed, m_rightSpeed;
	bool m_firstInertComp, m_firstMovingComp;
	int m_gear;
	SimplePID m_leftPID, m_rightPID;
	SimplePID m_deadheadPID;
	
	enum { kLoGear, kHiGear };
	
	DriveSystem();
	DriveSystem(BossRobot *);
private:
	void InitPID();
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
	virtual bool IsTurning();
protected:
	virtual void InitInertCompensate();
	virtual void InertCompensate();
	virtual void InitMovingCompensate();
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
	AutonomousDriveSystem(BossRobot *);
	virtual void ReadControls();
};

class TeleoperatedDriveSystem : public DriveSystem
{
protected:
	TeleoperatedDriveSystem() : DriveSystem() {}
	TeleoperatedDriveSystem(BossRobot *r) : DriveSystem(r) {}
	
	virtual void InterpretControls() = 0;
};

class ArcadeDriveSystem : public TeleoperatedDriveSystem
{
protected:
	float m_move, m_rotate;
	
	virtual void InterpretControls();
public:
	ArcadeDriveSystem(BossRobot *);
	virtual void ReadControls();
	virtual bool IsMoving();
	virtual bool IsTurning();
};

class TankDriveSystem : public TeleoperatedDriveSystem
{
protected:
	virtual void InterpretControls();
public:
	TankDriveSystem(BossRobot *);
	virtual void ReadControls();
};

class XboxDriveSystem : public ArcadeDriveSystem
{
public:
	XboxDriveSystem(BossRobot *);
	virtual void ReadControls();
};

#endif
