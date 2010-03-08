/**
 *	@file DriveSystem.hpp
 *	Header for all of the different drive systems.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 2/11/10.
 */

#include "WPILib.h"
#include "Options.hpp"
#include "BossRobot.hpp"
#include "SimplePID.hpp"

#ifndef _BOSS_973_DRIVESYSTEM_H_
#define _BOSS_973_DRIVESYSTEM_H_

/**
 *	The drive subsystem.
 *
 *	This keeps the motor speeds and controls encapsulated and easy to manage,
 *	along with providing drive compensation.
 */
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
	
	/**
	 *	Read controls from whatever source the drive system deems prudent.
	 *
	 *	This isn't defined in the base class, because there are many different
	 *	control schemes (including not having physical control).
	 */
	virtual void ReadControls() = 0;
	
	/** Send driving data to motors. */
	virtual void Drive();
	
	/**
	 *	Perform drive compensation.
	 *
	 *	This includes the "absolutely stationary" compensation and the
	 *	"deadheading" compensation.  This method should be called before #Drive,
	 *	or else the compensation won't be sent to the motors.
	 *
	 *	You shouldn't have to change this in subclasses; it is designed to be
	 *	generic.
	 */
	virtual void Compensate();
	
	/** Stop all motors. */
	virtual void Stop();
	
	/**
	 *	Check whether the drive system is moving.
	 *
	 *	DriveSystem provides a reasonable default, but subclasses (usually ones
	 *	with physical controls) may wish to override this.  This method is used
	 *	extensively in the #Compensate method.
	 *
	 *	@return If the drive is in motion
	 */
	virtual bool IsMoving();
	
	/**
	 *	Check whether the drive system is turning.
	 *
	 *	DriveSystem provides a reasonable default, but subclasses (usually ones
	 *	with physical controls) may wish to override this.  This method is used
	 *	extensively in the #Compensate method.
	 *
	 *	@return If the drive is turning
	 */
	virtual bool IsTurning();
protected:
	virtual void InitInertCompensate();
	virtual void InertCompensate();
	virtual void InitMovingCompensate();
	virtual void MovingCompensate();
};

/**
 *	A programmatic drive system.
 *
 *	This class is actually useful for modes outside of autonomous.
 *	The only thing this does is just allows driving programmatically, so you can
 *	use this if you don't want the operator to be able to drive for a while.
 */
class AutonomousDriveSystem : public DriveSystem
{
public:
	AutonomousDriveSystem(BossRobot *);
	virtual void ReadControls();
};

/**
 *	A human-controlled drive system.
 *
 *	This class is abstract, so don't create instances of it (not that you can
 *	in external code anyway).
 */
class TeleoperatedDriveSystem : public DriveSystem
{
protected:
	TeleoperatedDriveSystem() : DriveSystem() {}
	TeleoperatedDriveSystem(BossRobot *r) : DriveSystem(r) {}
	
	/**
	 *	Compute the left and right motor speeds.
	 *
	 *	It is important that this is separate from #ReadControls so that even
	 *	if a subclass radically changes which controls map to what, the logic
	 *	is the exact same.
	 */
	virtual void InterpretControls() = 0;
};

/**
 *	A human-controlled arcade drive system.
 *
 *	This will use joystick 1 for forward/backward and joystick 2 for turning.
 */
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

/**
 *	A human-controlled tank drive system.
 *
 *	This will use joystick 1 for left motor and joystick 2 for right motor.
 */
class TankDriveSystem : public TeleoperatedDriveSystem
{
protected:
	virtual void InterpretControls();
public:
	TankDriveSystem(BossRobot *);
	virtual void ReadControls();
};

/**
 *	A human-controlled drive system, using an Xbox controller.
 *
 *	Left analog is forward/backward, right analog is turning, left shoulder is
 *	low gear, and right shoulder is high gear.
 */
class XboxDriveSystem : public ArcadeDriveSystem
{
public:
	XboxDriveSystem(BossRobot *);
	virtual void ReadControls();
};

#endif
