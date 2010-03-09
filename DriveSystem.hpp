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
#include "Flag.hpp"

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
	Flag m_inertFlag, m_movingFlag;
	short m_gear;
	SimplePID m_leftPID, m_rightPID;
	SimplePID m_deadheadPID;
	
	DriveSystem();
	DriveSystem(BossRobot *);
private:
	void InitPID();
public:
	enum { kLoGear, kHiGear };
	
	virtual ~DriveSystem() {}
	
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
	
	/**
	 *	Drive a turn programmatically.
	 *
	 *	This is essentially the same as WPILib's RobotDrive::Drive, except
	 *	ported to work on our system.
	 *
	 *	@param speed
	 *		The forward/backward component of the drive, bounded in [-1.0, 1.0].
	 *	@param curve
	 *		The amount of curvature bounded in [-1.0, 1.0].  Negative values
	 *		take the robot to the left, positive values take the robot to the
	 *		right.
	 */
	virtual void Turn(float speed, float curve);
	
	/**
	 *	Set motor speeds programmatically.
	 *
	 *	This gives you full manual control of the robot's drive.  The drive
	 *	system will automatically flip the motor speeds appropriately, so that
	 *	1.0 means go forward, and -1.0 means go backward.  So something like
	 *	this:
	 *
	 *		<code>robot->GetDriveSystem()->SetSpeeds(1.0, 1.0);</code>
	 *
	 *	will make the robot go forward.
	 *
	 *	@param left
	 *		The speed of the left motors.
	 *	@param right
	 *		The speed of the right motors.
	 */
	virtual void SetSpeeds(float left, float right);
	
	/** Stop all motors. */
	virtual void Stop();
	
	/**
	 *	Retrieve the current gear.
	 *
	 *	@return The current gear setting
	 */
	inline short GetGear()
	{
		return m_gear;
	}
	
	/**
	 *	Change the drive gear.
	 *
	 *	@param gear
	 *		The new gear setting
	 */
	inline void SetGear(short gear)
	{
		m_gear = gear;
	}
	
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
