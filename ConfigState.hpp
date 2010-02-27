//
//	ConfigState.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/26/10.
//

#include "WPILib.h"
#include "State.hpp"
#include "Flag.hpp"

#ifndef _BOSS_973_CONFIGSTATE_H_
#define _BOSS_973_CONFIGSTATE_H_

class ConfigState : public State
{
protected:
	Flag m_reread;
	Flag m_strengthLo, m_strengthMd, m_strengthHi;
	Flag m_kickRest;
public:
	ConfigState(BossRobot *r);
	
	virtual void Enter();
	virtual void Exit();
	virtual void Step();
protected:
	void HandleStrengthPresetting();
};

#endif
