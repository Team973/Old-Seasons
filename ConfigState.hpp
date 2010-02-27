//
//	ConfigState.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/26/10.
//

#include "WPILib.h"
#include "State.hpp"

#ifndef _BOSS_973_CONFIGSTATE_H_
#define _BOSS_973_CONFIGSTATE_H_

class ConfigState : public State
{
protected:
	bool m_prevReread;
	bool m_prevStrengthLo, m_prevStrengthMd, m_prevStrengthHi;
	bool m_prevKickRest, m_prevKickDeadband;
public:
	ConfigState(BossRobot *r);
	
	virtual void Enter();
	virtual void Exit();
	virtual void Step();
protected:
	void HandleStrengthPresetting();
};

#endif
