//
//	NormalState.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/12/10.
//

#include "WPILib.h"
#include "State.hpp"
#include "SimplePID.hpp"

#ifndef _BOSS_973_NORMALSTATE_H_
#define _BOSS_973_NORMALSTATE_H_

class NormalState : public State
{
protected:
	SimplePID m_leftPID, m_rightPID;
	bool m_still, m_prevStill;
public:
	NormalState(BossRobot *r);
	
	virtual void Enter();
	virtual void Exit();
	virtual void Step();
};

#endif
