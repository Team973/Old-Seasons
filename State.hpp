//
//	State.hpp
//	Team 973
//	2010 "The Boss"
//
//	Created on 2/11/10.
//

#include "WPILib.h"
#include "BossRobot.hpp"

#ifndef _BOSS_973_STATE_H_
#define _BOSS_973_STATE_H_

class State
{
protected:
	BossRobot *m_robot;
	
	State()                { m_robot = NULL; }
	State(BossRobot *r)    { m_robot = r; }
public:
	virtual ~State() {};
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual void Step() = 0;
};

// TODO: Expand acronym without offending people
class GTFUState : public State
{
};

class FinaleState : public State
{
};

class RaisingState : public State
{
};

#endif
