//
//	NormalState.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/12/10.
//

#include "WPILib.h"
#include "State.hpp"

#ifndef _BOSS_973_NORMALSTATE_H_
#define _BOSS_973_NORMALSTATE_H_

class NormalState : public State
{
public:
	NormalState(BossRobot *r);
	
	virtual void Enter();
	virtual void Exit();
	virtual void Step();
};

#endif
