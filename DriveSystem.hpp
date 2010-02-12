// DriveSystem.hpp
// Created on 2/11/10.

#include "WPILib.h"
#include "BossRobot.hpp"

#ifndef _BOSS_973_DRIVESYSTEM_H_
#define _BOSS_973_DRIVESYSTEM_H_

class DriveSystem
{
protected:
	BossRobot *m_robot;
	RobotDrive *m_drive;
	
	DriveSystem();
	DriveSystem(RobotDrive *);
public:
	virtual ~DriveSystem() {}
	
	inline RobotDrive *GetDrive()
	{
		return m_drive;
	}
	
	virtual void ReadControls() = 0;
	virtual void Drive() = 0;
	virtual void Stop();
	virtual void Compensate();
};

/**
 * 	This class is actually useful for modes outside of autonomous.
 * 	The only thing this does is just allows driving programmatically, so you can
 *  use this if you don't want the operator to be able to drive for a while.
 */
class AutonomousDriveSystem : public DriveSystem
{
public:
	AutonomousDriveSystem(RobotDrive *);
	virtual void ReadControls();
	virtual void Drive();
};

class ArcadeDriveSystem : public DriveSystem
{
protected:
	float m_x, m_y;
public:
	ArcadeDriveSystem(RobotDrive *);
	virtual void ReadControls();
	virtual void Drive();
};

class XboxDriveSystem : public ArcadeDriveSystem
{
public:
	XboxDriveSystem(RobotDrive *);
	virtual void ReadControls();
};

#endif
