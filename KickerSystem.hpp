//
//	KickerSystem.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/16/10.
//

#include "WPILib.h"
#include "Options.hpp"
#include "BossRobot.hpp"
#include "Flag.hpp"

#ifndef _BOSS_973_KICKERSYSTEM_H_
#define _BOSS_973_KICKERSYSTEM_H_

/**
 *	The kicker subsystem.
 */
class KickerSystem
{
protected:
	BossRobot *m_robot;
	short m_strength;
	bool m_kicking;
	Flag m_inDeadband;
	Flag m_kickTrigger;
	
	enum {kStrengthLo, kStrengthMd, kStrengthHi};
public:
	KickerSystem(BossRobot *);
	virtual ~KickerSystem();
	
	/** Read the controls from the control board. */
	virtual void ReadControls();
	
	/** Send data to the kicker motors */
	virtual void Update();
	
	/**
	 *	Perform a kick.
	 *
	 *	If the robot is already making a kick, then this method will do nothing.
	 */
	virtual void Kick();
protected:
	virtual void UpdateWinch();
	virtual void UpdateKicker();
};

#endif
