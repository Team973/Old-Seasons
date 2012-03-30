/**
 *	@file ArmSystem.hpp
 *	Header for the ArmSystem class.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 3/8/10.
 */

#include "WPILib.h"
#include "Options.hpp"
#include "BossRobot.hpp"
#include "SimplePID.hpp"

#ifndef _BOSS_973_ARMSYSTEM_H_
#define _BOSS_973_ARMSYSTEM_H_

/**
 *	The arm subsystem.
 *
 *	The shoulder joint is on constant PID control and has three presets: stowed,
 *	raised, and GTFU.  You can change which position you want by calling
 *	#SetState.
 *
 *	Because the arm is mostly controlled by the program (not the user), there is
 *	no ReadControls method.  It is the programmer's responsibility to control
 *	the arm appropriately.
 */
class ArmSystem
{
protected:
	BossRobot *m_robot;
	SimplePID m_pidControl;
	short m_state;
	bool m_braked;
public:
	enum {kStowed, kRaised, kGTFU};
	
	ArmSystem(BossRobot *);
	virtual ~ArmSystem();
	
	/**
	 *	Set the desired position of the arm.
	 *
	 *	@param s
	 *		The new state.  Can be one of #kStowed, #kRaised, or #kGTFU.
	 */
	inline void SetState(short s)
	{
		m_state = s;
	}
	
	/** Update the arm motors. */
	virtual void Update();
	
	/** See whether we're moving. */
	virtual bool NeedsMove();
	
	/** Brake the system. */
	virtual void Brake();
	
	/** Unbrake the system. */
	virtual void Unbrake();
};

#endif
