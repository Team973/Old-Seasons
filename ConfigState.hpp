/**
 *	@file ConfigState.hpp
 *	Header for the ConfigState class.
 *
 *	Team 973<br>
 *	2010 "The Boss"
 *
 *	Created on 2/26/10.
 */

#include "WPILib.h"
#include "State.hpp"
#include "Flag.hpp"

#ifndef _BOSS_973_CONFIGSTATE_H_
#define _BOSS_973_CONFIGSTATE_H_

/** The configuration mode for the robot. */
class ConfigState : public State
{
protected:
	Flag m_reread;
	Flag m_strengthLo, m_strengthMd, m_strengthHi;
	Flag m_kickRest;
	Flag m_shoulderStowed, m_shoulderRaised, m_shoulderGTFU;
public:
	ConfigState(BossRobot *r);
	
	virtual void Enter();
	virtual void Exit();
	virtual void Step();
protected:
	void HandleStrengthPresetting();
	void HandleKickPresetting();
	void HandleShoulderPresetting();
};

#endif
