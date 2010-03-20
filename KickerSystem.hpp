/**
 *	@file KickerSystem.hpp
 *	Header for the KickerSystem class.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 2/16/10.
 */

#include "WPILib.h"
#include "Options.hpp"
#include "BossRobot.hpp"
#include "SimplePID.hpp"
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
	short m_strength, m_intakeState;
	bool m_kicking, m_startedKicking;
	bool m_cocking, m_cockingBegan, m_cockingEnded;
	SimplePID m_kickerPID;
	Flag m_kickTrigger, m_intakeFlag, m_resetFlag;
	
	Timer *m_intakeTimer;
public:
	enum {kStrengthLo, kStrengthMd, kStrengthHi};
	
	KickerSystem(BossRobot *);
	virtual ~KickerSystem();
	
	/** Read the controls from the control board. */
	virtual void ReadControls();
	
	/** Send data to the kicker motors */
	virtual void Update();
	
	/** Check whether the winch should be changed */
	virtual bool NeedsWinchUpdate();
	
	/** Reset all kick system values */
	void Reset();
	
	/** Reset kicker values */
	void ResetKicker();
	
	virtual void SetStrength(short s);
	
	/**
	 *	Set the kicker to the cocked position.
	 *
	 *	Yes.  Please laugh at this function's name.  It's the only place where I
	 *	will use the word in its shortened form.
	 */
	virtual void Cock();
	
	/** Turn on intake */
	virtual void RunIntake();
	
	/** Turn intake off */
	virtual void StopIntake();
	
	/** Check whether the kicker is posessing a ball. */
	virtual bool HasPossession();
	
	/**
	 *	Perform a kick.
	 *
	 *	If the robot is already making a kick, then this method will do nothing.
	 */
	virtual void Kick();
protected:
	virtual double GetWinchTarget();
	
	virtual void UpdateWinch();
	virtual void UpdateKicker();
	virtual void UpdateIntake();
};

#endif
